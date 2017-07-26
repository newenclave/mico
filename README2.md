# Mico

Monkey :monkey: the language interpreter implementation done with C++. https://interpreterbook.com/

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

### Compile

Mico is a header-only project. Well you need just compile `main.cpp`

```bash
 github $ git clone https://github.com/newenclave/mico.git
 github $ cd mico
 mico $ git submodule update --init etool
 mico $ g++ main.cpp -O2 -Iinclude -Ietool/include -o mico

```
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
Here `sum2` is a function that accepts 2 parameters (`a, b` for example) and returns `0 + a + b`
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
Here `parms` is a last parameter of the function and it is an array that contains all function's parameters.
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

    spin( 100_000 ) /// Oops. Stack overflow is here

    let spin = fn( count ) {
        if( count > 0 ) {
            return spin(count - 1) // return makes the tail call
        }
        "Ok"
    }
    spin( 100_000 ) /// optimized again.
```
