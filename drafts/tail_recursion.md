## tail recursion optimization for Monkey the language 	(https://interpreterbook.com/)


Step 1:

    In the object system I added a very special object that name is "continuation_call" (type: CONT_CALL).
    The object contains:
        1: function object
        2: environment with all of parameters set for this function

    The object must be returned from scope evaluator (evalStatements)
        when the last expression or return are reached.
        something like this: (pseudocode)
    ```
    func evalStatementsImpl ( stmts, env ) -> object
    {
        var result null_obj
        for(statement in stmts) {
            if( statement is RETURN ) {
                if( statement.expression is CALL ) {
                    rerurn newContCall{ Function = statement.expression.Function,
                                        Env = makeCallEnv(Function, env) }
                }
            }
            if( ISLAST && statement is CALL ) { /// is the statement is the last one and it is a CALL
                rerurn newContCall{ Function = statement.Function,
                                    Env = makeCallEnv(Function, env) }

            }
            result = Eval(statement)
            if returnValue, ok := result.(*object.ReturnValue); ok {
                return returnValue.Value
            }
        }
        return result
    }
    ```
    https://github.com/newenclave/mico/blob/master/include/mico/eval/tree_walking.h#L447


    Here: make_env just set up new enviroment for the contCall.

    Well now we can rewrite evalStatements

    ```
    func evalStatements ( stmts, env ) -> object
    {
        var result = evalStatementsImpl( stmts, env )
        while (result is CONT_CALL) {
            result = evalStatementsImpl( result.Func.Body, result.Env )
        }
        return result
    }
    ```

    And that is all.
    Now we can replace evalStatements in "evalIfExpression"
    ```
    if isTruthy(condition) {
        return EvalImpl(ie.Consequence) // returns CONT_CALL if it is found
    } else if ie.Alternative != nil {
        return EvalImpl(ie.Alternative) // returns CONT_CALL if it is found
    } else {
        return NULL
    }
    ```

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
    This code wil not failed with TRO and will without.

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
