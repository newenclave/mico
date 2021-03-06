﻿## tail call optimization for Monkey the language (https://interpreterbook.com/)


Step 1:

In the object system I added a very special object with type "CONT_CALL".

    The object contains:
        1: function object
        2: environment with all of parameters set for this function

```
    type ContCallLiteral struct {
        FunctionLiteral *Function
        Env *Environment
    }
```

Step 2:

The object must be returned from the scope evaluator (evalStatements) when the last expression or return are reached.
Something like this: (some pseudocode; so far as I use C++)

```
    func evalStatementsImpl ( stmts, env ) -> object
    {
        var result null_obj
        for(statement in stmts) {
            if( statement is RETURN ) {
                if( statement.expression is CALL ) {
                    rerurn newContCall{ Function = statement.expression.Function,
                                        Env = makeContCallEnv(Function, env, statement) }
                }
            }
            if( ISLAST && statement is CALL ) { /// is the statement the last one and is a CALL
                rerurn newContCall{ Function = statement.Function,
                                    Env = makeContCallEnv(Function, env, statement) }

            }
            result = Eval(statement)
            if returnValue, ok := result.(*object.ReturnValue); ok {
                return returnValue.Value
            }
        }
        return result
    }
```

https://github.com/newenclave/mico/blob/master/include/mico/eval/tree_walking.h#L455


Here: makeCallEnv just set up new enviroment for the contCall.


```
    func makeContCallEnv(FunctionLiteral *func, env *Environment, node ) -> *Environment
    {
        args := evalExpressions(node.Arguments, env)
        if len(args) == 1 && isError(args[0]) {
            //// failed
            rerturn null; /// hmm
        }
        return extendFunctionEnv(func, args)
    }
```

Step 3: Well, now we can rewrite evalStatements

```
    func evalStatements ( stmts, env ) -> object
    {
        var result = evalStatementsImpl( stmts, env )
        while (result is CONT_CALL) {
            result = evalStatementsImpl( result.Function.Body, result.Env )
        }
        return result
    }

```

Step 4:

And that is all. Now we can replace evalStatements in "evalIfExpression"

```
    if isTruthy(condition) {
        return EvalImpl(ie.Consequence) // returns CONT_CALL if it exists
    } else if ie.Alternative != nil {
        return EvalImpl(ie.Alternative) // returns CONT_CALL if it exists
    } else {
        return NULL
    }
```

Step 5: tests

And here we go.

```
    let x = fn(count){
        if(count > 0) {
            x(count - 1)
        } else {
            0
        }
    }
    x(0x7FFFFFFFFFFFFFFF)
```

or

```
let x = fn( ){ x( ) }
x( )
```

Works!


=================

factorial and fibonacci

```
    let fac = fn(val) {
        let impl = fn( val, acc ) {
            if( val > 1 ) {
                impl( val - 1, acc * val )
            } else {
                acc
            }
        }
        impl( val, 1 )
    }

    let fib = fn( n ) {
        let impl = fn( a, b, n ) {
            if(n > 0) {
                impl( b, a + b, n -1 )
            } else {
                a
            }
        }
        impl(0, 1, n)
    }
```

#### some words about C++ implementation

The ```enviroment``` objects contains a map that binds object->object and a vector of children.
Child is a ```shared_ptr<enviroment>```.
Also ```enviroment``` contains a ```weak_ptr<enviroment>```. This is a parent.
In the destruntor of the function object (or cont_call object)
the enviroment goes to  the parent if it exists and removes itself from
the parent object.
