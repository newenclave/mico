# Mico

<a href="https://scan.coverity.com/projects/newenclave-mico">
  <img alt="Coverity Scan Build Status"
       src="https://img.shields.io/coverity/scan/13291.svg"/>
</a>

Monkey :monkey: the language interpreter implementation done with C++. https://interpreterbook.com/

[Old README](README_old.md)

![Mico REPL Terminal](repl.png)

## Requirements

* C++ compiler (c++11 or later)
* The book https://interpreterbook.com/. If you have not read it yet.

## Table of Contents

* [Compile](#compile)
* [Monkey and Mico](#monkey-and-mico)
* [View](#view)
    * [Numbers](#numbers)
    * [Token position](#token-position)
    * [Mutability](#mutability)
    * [if elif else](#if-elif-else)
    * [Operators](#operators)
* [Functions](#functions)
    * [First-class Citizen](#first-class-citizen)
    * [Partial application](#partial-application)
    * [Variadic parameters](#variadic-parameters)
    * [Pipe operator](#pipe-operator)
    * [Tail Call Optimization](#tail-call-optimization)
* [A Macro System For Monkey](#a-macro-system-for-monkey)
    * [Expression quote](#expression-quote)
    * [Statements quote](#statements-quote)
    * [macro](#macro)
    * [Builtin Macroses](#builtin-macroses)
* [Modules](#modules)
    * [Inheretence](#inheretence)

### Compile

Mico is a header-only project. Well you need just compile `mico.cpp`

```bash
 github $ git clone https://github.com/newenclave/mico.git
 github $ cd mico
 mico $ git submodule update --init etool
 mico $ g++ mico.cpp -O2 -Iinclude -Ietool/include -Wall -o mico
```
Or clang
```bash
 mico $ clang++ -O2 mico.cpp -std=c++11 -Iinclude -Ietool/include -Wall -o mico
```
Or MS SDK compiler. It sould be called from the "Visual Studio Command Prompt" for example
```bash
mico> cl /I include /I etool/include mico.cpp
mico> link /OUT:"mico.exe" mico.obj
```
Or just use Visual Studio (at least version 12)

Thats all.

### Monkey and Mico
"Mico" is an implementation but of course it has some difference.
For now I'm pretty sure that Mico can run the major part of Monkey's code.

### View
Mico supports: integers, floats, strings, arrays, tables, functions
```swift
    let int = 1_000_000                     // int is an integer
    let f   = 0.1e-3                        // f is a float `0.0001`
    let s   = "This is a string"            // string
    let t   = { "x": 0, "y": 0, "z": 0 }    // table (or Hash)
    let a   = [1, 2, 3, 4, 5, 6, [0, 0, 0]] // array (that contains another array)
    let fun = fn(a, b){ (a - b) * (a + b) } // function
```
#### Numbers
Numbers can contain a gap symbol `_`. The symbol can be included both in integers and in floats.
```swift
    let int = 1_000_000             // 1000000
    let hex = 0xAAA_BBB             // 11185083
    let bin = 0b1111_0000_1111_0000 // 61680
    let ter = 0t22_11_22_11         // 6232
    let oct = 0_777_111_222_333     // 68604470491
    let flt = 10.1000_0002          // 10.10000002
```
#### Token position
Every token has its position.
```swift
    let test_val = someUnknownCall()
    error: [1:15] Identifier not found 'someUnknownCall'
```

#### Mutability
Every variable is a real variable and can be changed by assignmet operator `=`.
Arrays' values also can be changed by the operator
```swift
    let a = [1, 2, 3, 4, 6]
    let b = 1
    a[4] = 5
    b = 10
    c = 100 /// oops; error: [1:0] Identifier not found: 'c'

    let fun = fn( ) {
        let a = 100 // here `a` shadows the variable `a` from the global scope
        b = 0.0     // here `b` is from the global scope
    }
    fun( )
```
The operator is a `right arm` operator.
```swift
    let a = [1, 2, 3, 4, 5]
    a[0] = a[1] = a[2] = a[3] = a[4] = 0
    // a == [0, 0, 0, 0, 0]
```

#### if elif else
```swift
    let x = 1000;
    let res = if( x < 0 ) {
                "too small!"
              } elif(x == 0) {
                "zero"
              } elif( x < 10 ) {
                "less than 10"
              } elif( x < 100 ) {
                "less than 100"
              } else {
                "too big!"
              }
```

#### Operators
Mico has: +, -, ==, !=, <, >, <=, >=, %, |, &, ^, &&, ||, >>, <<, ~
```swift
    let bits = fn( value ) {
        let impl = fn( val, acc ) {
            if( val > 0 ) {
                impl( val >> 1, acc + (val & 1) ) // or val % 2
            } else {
                acc
            }
        }
        impl( value, 0 )
    }
    bits(0xFFFFFFFFF)               // => 36
    bits(0b1111111000000001111111)  // => 14
```
Operators `&&` and `||` are lazy. `&&` doesn't eval its right side if its left side is `false`.
`||` does the same thing. It returns `true` if its left side is `true`

```swift
    if( 10 > 100 && "100" < 100 ) { 1.0 } else { 0.0 } // ok. returns 0.0
    if( 10 > 100 || "100" < 100 ) { 1.0 } else { 0.0 }
    // error: [1:22] Infix operation '<' is not defined for string and integer
```
### Functions
#### First-class Citizen
All function in Monkey (i.e. in Mico also) are [first-class citizens](https://en.wikipedia.org/wiki/First-class_citizen)
```swift
    let sum = fn(a, b) {
        a + b
    }
    let apply = fn( call, a, b ) {
        call(a, b)
    }
    let bind  = fn( call, a ) { // returns a function that accepts 1 parameter
        fn( b ) {
            apply( call, a, b )
        }
    }
    let inc = bind(sum, 1)
    let res = inc(10) // res == 11
```
#### Partial application
[Partial application](https://en.wikipedia.org/wiki/Partial_application)

```swift
    let sum  = fn(a, b, c) { a + b + c}
    let sum2 = sum(0)
    let res  = sum2(1, 2) // res == 3
```
Here `sum2` is a function that accepts 2 parameters (`a, b` for example) and returns `0 + a + b`.

A function can be "restored" from its partial from. The prefix operator `*` does it.

```swift
    let sum  = fn(a, b, c) { a + b + c}
    let sum2 = sum(0)
    let res0 = sum2(2, 3)       // res0 = 5
    let res1 = (*sum2)(1, 2, 3) // res1 = 6
```
#### Variadic parameters
Functions in Mico can accept a variable number of arguments.
```swift
    let param_count = fn( ...parms ) { // prefix operator elipsis must be the last one
        return len(parms)
    }
    let seven = param_count( 1, 2, [], {}, fn(){ }, 6, 7 )
    let zero  = param_count( )
    puts("seven is ", seven, " and zero is ", zero)
```
This code shows `seven is 7 and zero is 0`.
Here `parms` is the last parameter of the function and it's an array that contains all function's parameters.
The array can be empty.

The elipsis doesn't have a role in [partial application](#partial-application).
It is "invisible" for the partial application system
```swift
    let reduce = fn( call, arr ) {
        let impl = fn( arr, acc, id ) {
            if( id < len(arr) ) {
                impl( arr, call(acc, arr[id]), id + 1 )
            } else {
                acc
            }
        }
        impl( arr, 0, 0 )
    }
    let sum = fn( a, b, ...args ) {
        let arr_sum = reduce( fn(a, b){ a + b } ) // partial
        a + b + arr_sum(args)
    }
    let sum_0    = sum(1, 2)           // 3
    let sum_1    = sum(1, 2, 3)        // 6
    let sum_2    = sum(1, 2, 3, 4, 5)  // 15
    let sum_part = sum(0.001)          // `sum_part` is a function with 1 parameter + `...`
    let sum_3    = sum_part( 1, 2, 3 ) // => sum( 0.001, 1, 2, 3 ) => 6.001
```

#### Pipe operator
The pipe operator `|` is a shortcat for the `call` operator `()`.
The left side of the operator is passed to the right side.
In this case the right side must be a function (builtin or not).
```swift
    let sum = fn( a, b ) { a + b }
    let hello = sum("Hello, ")      // fn( a ) { "Hello, " + a } // partial

    "Hello, world!" |len            // => 13
    "world!" |hello                 // sum("Hello, ", "world!") => "Hello, world!"
    "world!" |sum("Hello, ") |len   // len(sum("Hello, ", "world!")) => 13

    /// check this out =)
    let x = 10
    "world!" |if(x > 0){hello} else {(*hello)("Goodbye, ")} // => Hello, world!
    ......
    let x = 0
    "world!" |if(x > 0){hello} else {(*hello)("Goodbye, ")} // => Goodbye, world!

```

#### Tail Call Optimization
Well. Mico supports it. [Tail Call](https://en.wikipedia.org/wiki/Tail_call)

```swift
    let spin = fn( count ) {
        if( count > 0 ) {
            spin(count - 1) // a tail call
        } else {
            "Ok"
        }
    }
    spin( 0xFFFF_FFFF ) // Ok, Here is a tail call

    let spin = fn( count ) {
        if( count > 0 ) {
            spin(count - 1) // not a tail call, beause of "Ok" expression
        }
        "Ok"
    }

    spin( 100_000 )
    /// Stack overflow is here!
    /// error: [3:12] Stack overflow spin((count-1))

    let spin = fn( count ) {
        if( count > 0 ) {
            return spin(count - 1) // return makes the tail call
        }
        "Ok"
    }
    spin( 100_000 ) /// optimized again.

    let call1 = fn(c) {
        puts("1 ", c)
        if( c > 0 ) {
            call2(c - 1)
        } else {
            "Ok 1"
        }
    }
    let call2 = fn(c) {
        puts("2 ", c)
        if( c > 0 ) {
            call1(c - 1)
        } else {
            "Ok 2"
        }
    }
    call1(0xFFFFFF) // Ok
```
### A Macro System For Monkey


Yes. Finally I've had some free time to add the system; https://interpreterbook.com/lost/
So. First of all **quote**, **unquote**, and **macro** are keywords and I can parse them as I wish.
And there are 2 types of **quote**

#### Expression quote
```swift
    /// expression quote = quote + ( + expression + )

    let a = quote( 2 + 2 ) /// `a` is an ast node and contains (2 + 2)
    unquote(a) // => ( 2 + 2 ) => 4
    quote(unquote(quote(unquote(quote(unquote(quote(unquote(2+2)))))))) // => quote(4) =)

    /// unquote can be placed wherever you want.

    /// function body
    let x = 100 /// just a value for unquote the call

    let quote_func =  quote( fn( a ) { unquote(10 * x) + a } )
    //  quote_func => quote( fn( a ) { 1000            + a } )

    let quote_if =  quote( if( unquote(x) > 10 ) { "huge" } else { "small" } )
    //  quote_if => quote( if( 100        > 10 ) { "huge" } else { "small" } )

    let x = "Hello!"
    let quote_call =  quote( unquote(x) | len )
    /// quote_call =  quote( "Hello!"   | len )

    unquote(quote_func)(10) // 1010
    unquote(quote_if)       // "huge"
    unquote(quote_call)     // 6

```
#### Statements quote
```swift

    /// statements quote = quote + { + statements + }

    let a = quote { let a = 1000; let b = a * 2 } // `a` is an ast node
                                                  // and contains 2 `let` statements
    unquote(a) // => eval `let a = 1000; let b = a * 2` => null
               // now `a` is 1000 and `b` is 2000
```
#### macro

The `macro` keyword defines a piece of code that is placed in the position where it is called.
And this is all happening in the macro expansion phase before the evaluation.

```swift
    let sum = macro( a, b ) { unquote(a) + unquote(b) }
    sum(10, 200)  // here the `ast` is replaced by body of the macro sum so => 210

    let set_env = macro( env ) { unquote(env) }

    set_env( quote {
        let a = 10
        let b = 20
    } )
    /// now a is 10 and b is 20
    sum(a, b) // => 30 yeah! inline (a + b) =)

    /// macro `test` stores the macro `sum` and it is inlined when `test` is called
    let test = macro( a ) { sum(10, unquote(a) ) }
    test(10) /// ast is ( 10 + 10 )

    let test2 = macro( a, b ) {
        let impl = macro( a, b ) {  /// yes! macroses have their scopes!
                                    /// impl can be accessed only in `test2`
            unquote(a) + unquote(b) /// variable shadowing...yes here is
                                    /// `a` and `b` shadow
                                    /// `a` and `b` from the scope of test2
        }
        impl(unquote(a), unquote(b))
    }
    test2(10, 20) // just inlined (10 + 20) => 30 of course

    /// and `The Mighty Unless Macro` of course
    let unless = macro(condition, consequence, alternative) {
        puts("Here the macro inlined!")
        if(!(unquote(condition))) {
            unquote(consequence);
        } else {
            unquote(alternative);
        };
    }
    unless(10 > 5, puts("not greater"), puts("greater"))
    // Here the macro inlined!
    // greater
    // =)

    /// just for debug purposes
    __macro( ) // shows all the macroses stored by REPL

    // Macros:
    // set_env => macro(env) {
    //  unquote(env)
    // }
    // ==========
    // sum => macro(a, b) {
    //  (unquote(a)+unquote(b))
    // }
    // ==========
    // test => macro(a) {
    //  sum(10, a)
    // }
    // ==========
    // test2 => macro(a, b) {
    //  let impl = macro(a, b) {
    //      (unquote(a)+unquote(b))
    //  };
    //  impl(unquote(a), unquote(b))
    // }
    // ==========
    // unless => macro(condition, consequence, alternative) {
    //  puts("Here the macro inlined!");
    //  if ((!unquote(condition))) {
    //      unquote(consequence)
    //  } else {
    //      unquote(alternative)
    //  }
    // }
    // ==========

    // And one more thing...
    let ident = macro( ){ test_sum }
    let ident( ) = fn(a,b,c){ a + b + c };
    let apply = fn(call, a,b,c) { call(a,b,c) }
    apply(ident(), 1, 2, 3 ) /// Yeee-haa! we have made an ident!
    test_sum(1,2,3)          /// call

    __env( ) // shows us the environment

    /// Root: [0x560858819c30]
    ///   __env => fn(0x56085881a030) [0x560858819c30]
    ///   __macro => fn(0x56085881a160) [0x560858819c30]
    ///   apply => fn(4) [0x56381c7409a0]
    ///   copy => fn(0x560858819f00) [0x560858819c30]
    ///   len => fn(0x560858819ce0) [0x560858819c30]
    ///   puts => fn(0x560858819df0) [0x560858819c30]
    ///   test_sum => fn(3) [0x56085881ed10] // here is our function
    ///   Child: 1 [0x56085881d8a0]
    ///   Child: 1 [0x56085881ed10]

```
#### Builtin Macroses
Yes, it is possible to add them. One test macro `__concat_idents` I've added to the macro processor.
It just returns a concatination of all of the parameters it has. Well the parameter must be an ident or a number.
```js
    let __concat_idents(name, 1) = "this is the first one"

    puts(name1)
    puts(__concat_idents(name, 1)) // also should work
    /// this is the firts one

    puts(__concat_idents(name, 2))
    /// error: [1:29] Identifier not found 'name2'

    let __concat_idents(sum, 2) = fn(a,b) { a + b }
    __concat_idents(sum, 2)(1, 2) /// => sum2(1, 2) => 3
    sum2(1, 2) // => 3

```

### Modules
Modules are just pieces of the environment.
Modules have names and all elements of the module are available outside using the names.
They are more like `namespaces` in c++ but modules are first-class citizens.

```swift

    module a {
        let value = "value a"
        let show = fn( ) { puts( value ) }
    }
    a.show( ) // shows `value a`
    module b {
        let a = a // shadowing
        let show = fn( ) { puts( "module b: " + a.value ) }
    }
    b.a.show( ) // same as a.show( )
    b.show( )   // `module b: value a`

    let show = fn( mod ) { // accepts module like a parameter
        mod.show( )
    }

    show( a ) /// `value a`
    show( b ) /// `module b: value a`

```

#### Inheretence
A module can inherit values from another module. In the case of inheritence
all elements of parent are available in child. And by the child.
```swift
    module a {
        let value = "value a"
        let show = fn( ) { puts( value ) }
        let set = fn( val ) { value = val }
    }

    module b: a { // Inheretence
    }

    b.show( ) // `value a`
    b.set( "changed!" )
    puts(a.value)
    a.show( )
```
A module can have one or more parents.

```swift
    module a {
        let value = "value a"
        let showa = fn( ) { puts( value ) }
        let seta = fn( val ) { value = val }
    }

    module b { // Inheretence
        let value = "value b"
        let showb = fn( ) { puts( value ) }
        let setb = fn( val ) { value = val }
    }

    module c: a, b { }

    c.showa( ) // `value a`
    c.showb( ) // `value b`

```
And what is here?
```swift
    puts(c.value)
```
Well here is `value a`. Because module `a` is the first in the inheretence tree.
