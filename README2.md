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
Monkey has: +, -, ==, !=, <, >, <=, >=, %, |, &, ^, &&, ||, >>, <<, ~
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
    if( 10 > 100 && "100" < 100 ) { 0.005 } else { 0.0 } // ok. returns 0.0
    if( 10 > 100 || "100" < 100 ) { 0.005 } else { 0.0 }
    // error: [1:22] Infix operation '<' is not defined for string and integer
```

### Tail Call Optimization
It was the first thing I added to the interpreter. [Tail Call](https://en.wikipedia.org/wiki/Tail_call)

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
