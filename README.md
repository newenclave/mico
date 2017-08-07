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

# Table of Contents

* [Compile](#compile)
* [Monkey and Mico](#monkey-and-mico)
* [View](#view)
    * [Identifiers](#identifiers)
    * [Token position](#token-position)
    * [Numbers](#numbers)
    * [Strings](#strings)
    * [Raw strings](#raw-strings)
    * [Slices](#slices)
    * [Negative index](#negative-index)
    * [Mutability](#mutability)
        * [mut keyword](#mut-keyword)
        * [const keyword](#const-keyword)
    * [Intervals](#intervals)
    * [if elif else](#if-elif-else)
    * [for in](#for-in)
        * [break and continue keywords](#break-and-continue-keywords)
    * [Operators](#operators)
        * [Operator in](#operator-in)
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
    * [Inheritance](#inheritance)
    * [Anonymous](#anonymous)
* [Standart Library](#standart-library)
    * [io](#io)
    * [string](#string)
    * [dbg](#dbg)

## Compile

Mico is a header-only project. Well you need just compile `mico.cpp`

```bash
 github $ git clone https://github.com/newenclave/mico.git
 github $ cd mico
 mico $ git submodule update --init etool
 mico $ g++ -std=c++11  mico.cpp -O2 -Iinclude -Ietool/include -Wall -o mico
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

## Monkey and Mico
"Mico" is an implementation but of course it has some difference.
For now I'm pretty sure that Mico can run the major part of Monkey's code.

## View
Mico supports: integers, floats, chars, strings, arrays, tables, functions, modules, intervals
```swift
    let int  = 1_000_000                     // int is an integer
    let f    = 0.1e-3                        // f is a float `0.0001`
    let s    = "This is a string"            // string
    let t    = { "x": 0, "y": 0, "z": 0 }    // table (or Hash)
    let a    = [1, 2, 3, 4, 5, 6, [0, 0, 0]] // array (that contains another array)
    let fun  = fn(a, b){ (a - b) * (a + b) } // function
    let mod  = module { let var1 = 1; let var2 = 2 } // module
    let ival = 1..100                                // interval
    let c    = '🐒'     // character
```

### Identifiers
Identifiers can use unicode symbols
```swift
    let кирилица = "Кирилица"
    let españa   = "Spain"
    let 中國      = 12312
    let العربية
        = {1: 中國}

    io.puts(кирилица)
    io.puts(españa)
    io.puts(中國)
    io.puts(العربية
            [1])

    // Кирилица
    // Spain
    // 12312
    // 12312
 ```

### Token position
Every token has its position.
```swift
    let test_val = someUnknownCall()
    error: [1:15] Identifier not found 'someUnknownCall'
```

### Numbers
Numbers can contain a gap symbol `_`. The symbol can be included both in integers and in floats.
```swift
    let int = 1_000_000             // 1000000
    let hex = 0xAAA_BBB             // 11185083
    let bin = 0b1111_0000_1111_0000 // 61680
    let ter = 0t22_11_22_11         // 6232
    let oct = 0_777_111_222_333     // 68604470491
    let flt = 10.1000_0002          // 10.10000002
```
### Strings
```swift
    let s = "string"
    io.puts(s[1])
    // `t`
    let s = "中國"
    io.puts(s, " ", len(s), " ", s[0])
    // `中國 2 中`
```
Yes. It supports the unicode. Not completely of course.
I've written a small utf8 parser and I think it's enough for this toy language.
And for Windows it uses native API for encoding.

### Raw strings
Well. Raw strings are just arrays of bytes
```swift
    let s = r"string"
    io.puts(s[1])
    // 116
    let s = r"中國"
    io.puts(s, " ", len(s), " ", s[0])
    // 中國 6 228
```
Unlike strings raw strings know nothing about the unicode. And that why string `中國` has length 2 and the
lenght of the raw string is 6.

### Slices
Slice is a part of an array or a string. Slice holds the object (string or array) and an interval `[left..right]`.
The slice can be created with `index` operator `[]`. Interval includes only its left side (i.e. the intervals are `left closed, right open`).
Full container interval can be created with the interval `cont[0..len(cont)]` or `cont[0..-1]` (see "Negative index" above)
```swift
    let str = "This is a string"
    let this = str[0..4]
    io.puts(this)
    // shows `This`
```
Arrays slices
```swift
    let arr = [0,1,2,3,4,5,6,7,8,9]
    let s = arr[0..3]
    for i in s { io.put( i, " " ) } // shows `0 1 2`
```
Slice can change its direction. If the left part of the interval is greater then right, interval changes its direction.
```swift
    let arr = [0,1,2,3,4,5,6,7,8,9]
    let s = arr[8..2]
    for i in s { io.put( i, " " ) } // shows `7 6 5 4 3 2`
```
Of cource it's only valid when an element is accessed by index
```swift
    let str = "Hello, world!"
    let s = str[len(str)..0]
    for i in s { io.put( i ) } // shows `!dlrow ,olleH`
    /// but!
    io.puts(s) // Hello, world!
```
Slices are not copyes!

Slice can be also created from another slice.
```swift
    let arr = [0,1,2,3,4,5,6,7,8,9]
    let s = arr[1..10] // s is a slice[1,2,3,4,5,6,7,8,9]
    let t = s[2..5]    // t is a slice[3, 4, 5]
    for i in t { io.put( i, " " ) } // shows `3 4 5`
```

### Negative index
Elements of arrays or strings can be obtained by negative index. `-1` means `the last element of the array/string`
```swift
    let arr = [0,1,2,3,4,5,6,7,8,9]
    io.puts(arr[-1]) // 9
    io.puts(arr[-2]) // 8
    let s = "中國"
    io.puts(s[-2]) // 中
```
Well, slices also can be created by adding a negative index
```swift
    let str = "Hello, world!Äáç¶"
    let sym = str[-5..-1]
    let hello = str[0..-5]
    io.puts(sym) // Äáç¶
    io.puts(hello) // Hello, world!
```

### Mutability
By-default all values set by `let` statement are inmutable. For making a variable, that can change its value use `var` statement.
Every variable is a real variable and can be changed by assignmet operator `=`.

```swift
    var a = [1, 2, 3, 4, 6]
    var b = 1
    let d = 77.77 // `d` is constant
    b = 10        // ok
    c = 100       // oops; error: [1:0] Identifier not found: 'c'
    d = 88.88     // oops; error: [1:2] Invalid left value for ASSIGN d

    let fun = fn( ) {
        let a = 100 // here `a` shadows the variable `a` from the global scope
        b = 0.0     // here `b` is from the global scope
    }
    fun( )
```
#### mut keyword
All objects are also constats. It can be changed by keyword `mut`. It makes sense for arrays and tables.
```swift
    let a = [1,2,3,4]
    a[0] = 1000
    // error: [2:5] Invalid left value for ASSIGN a[0]
    // as far as the object is const
    let a = mut [1,2,3,4]
    a[0] = 1000     // ok
    io.puts(a[0])   // shows `1000`
```

#### const keyword
The `const` keyword can make an object constant.
```swift
    let a = mut [1,2,3,4]
    a[0] = 1000     // ok
    let a = const a // set const
    a[0] = 1000
    // error: [1:5] Invalid left value for ASSIGN a[0]
```

The keywords `mut` and `const` always make a copy of the object if the mutablity of the object is different.
```swift
    let a = mut [1,2,3,4]
    let a = mut a       // returns `a` without changes
    let a = const a     // makes a copy of `a`
```

The operator is a `right arm` operator.
```swift
    var a = [1, 2, 3, 4, 5]
    a[0] = a[1] = a[2] = a[3] = a[4] = 0
    // a == [0, 0, 0, 0, 0]
```
The `let` statement makes constants objects
```swift
    let a = [1, 2, 3, 4, 5]
    a[0] = a[1] = a[2] = a[3] = a[4] = 0
    // error: [2:9] Invalid left value for ASSIGN a[0]
```

### Intervals

Intervals are pairs of values. Values can be integers, floats, strings or boolean. Operator `..` creates an interval.
For now there are not a lot of operations with intervals.
```swift
    let float   = 1.0..100.0
    let int     = 1..100
    let bool    = false..true
    let strings = "a".."z"
```

### for in
The operator makes a for-loop. It's an expression and always returns its `value` as the result. There are several types of the operator.

Simple counter loop. Accepts an integer or a float as a `value` and makes a loop that is repeated `value` times
```swift
    for i in 10 {
        io.put(i, " ")
    }
    io.puts( )
```
This code shows `0 1 2 3 4 5 6 7 8 9`.

The operator can have a `step` value that can change loop's increment and even make it negative
```swift
    for i in 10, 2 {
        io.put(i, " ")
    }
    io.puts( )
    // 0 2 4 6 8

    for i in -7, -0.7 {
        io.put(i, " ")
    }
    io.puts( )
    // 0 -0.7 -1.4 -2.1 -2.8 -3.5 -4.2 -4.9 -5.6 -6.3
```
An interval loop acccepts a numeric (float or integer) interval and repats `[start..stop]` times. It means that this type of the loop includes right side of the interval.
```swift
    for i in 1..10 {
        io.put(i, " ")
    }
    io.puts( )
    // 1 2 3 4 5 6 7 8 9 10

    // the `reverse` variant is also available

    for i in 0.15..-12, -1.66 {
        io.put(i, " ")
    }
    io.puts( )
    // 0.15 -1.51 -3.17 -4.83 -6.49 -8.15 -9.81 -11.47

    for i in -12..0.15, 1.66 {
        io.put(i, " ")
    }
    io.puts( )
    // -12 -10.34 -8.68 -7.02 -5.36 -3.7 -2.04 -0.38
```
As you can see values `-13.13` (next to -11.47 for first) and `1.28` (next to -0.38 for second) are not shown because they lie outside the intervals


A container loop accepts containers (i.e. arrays, strings, slices or tables) and iterates those values.
```swift
    for v in [1, "Hello", 0.19, -100] {
        io.put(v, " ")
    }
    io.puts( )
    // 1 Hello 0.19 -100

    for v in {1: 1, 2: -0.2, 3: "zero.3"} {
        io.put(v, " ")
    }
    io.puts( )
    // zero.3 -0.2 1
    // yep. table doesn't have an order
```
For loop that accepts an array and a string it's also possible to use negative `step`.
```swift
    for v in [1, 2, 3, 4, 5], -1 {
        io.put(v, " ")
    }
    io.puts( )
    // 5 4 3 2 1

    // And of course it doesn't have to be `1` or `-1`
    for v in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10], -2 {
        io.put(v, " ")
    }
    io.puts( )
    // 10 8 6 4 2

    for i in "Кирилица, latinic, 中國 จีน มีตัวละครจ πολλοί عرب ٹیسٹ χαρακτήρες", -1 {
        io.put(i)
    }
    io.puts( )
    // ςερήτκαραχ ٹسیٹ برع ίολλοπ จรคะลวัตีม นีจ 國中 ,cinital ,ацилириК

    for i in "Кирилица, latinic, 中國 จีน มีตัวละครจ πολλοί عرب ٹیسٹ χαρακτήρες", -3 {
        io.put(i)
    }
    io.puts( )
    // ςήαχس عοοจะัมี國,na,ир

    // raw string are also itarables
    for i in r"Кирилица, latinic, 中國 จีน มีตัวละครจ πολλοί χαρακτήρες", -1 {
        io.put(i, " ")
    }
    io.puts( )
    // 130 207 181 206 129 207 174 206 132 207 186 206 177 206 129 207 177 206
    // 135 207 32 175 206 191 206 187 206 187 206 191 206 128 207 32 136 184
    // 224 163 184 224 132 184 224 176 184 224 165 184 224 167 184 224 177
    // 184 224 149 184 224 181 184 224 161 184 224 32 153 184 224 181 184
    // 224 136 184 224 32 139 156 229 173 184 228 32 44 99 105 110 105 116
    // 97 108 32 44 176 208 134 209 184 208 187 208 184 208 128 209 184 208 154 208
```

For `table` loops `step` is not available.

Key-Value syntax.

```swift
    for k, v in [8,8,8,8,8,8,8,8,8] {
        io.put(k, ":'", v, "' ")
    }
    io.puts( )
    /// 0:'8' 1:'8' 2:'8' 3:'8' 4:'8' 5:'8' 6:'8' 7:'8' 8:'8'

    // for the tables key is the value of the current table key.
    for k, v in { "x": 0, "y": -100, "z": -1, "t": "00:00:00" } {
        io.put(k, ":'", v, "' ")
    }
    io.puts( )
    // z:'-1' t:'00:00:00' y:'-100' x:'0'
```
Containers loops always set next value variable of the loop as a reference if the `value` is mutable.
It means that values in the container can be changed in the loop. And as far as loops are expressions we can easily create and change a container in the place

```swift
    // helper function. Shows a container value
    let show = fn( arr ) { for i in arr  { io.put( i, " " ) } io.puts( ) }

    let r = const for v in mut [1,2,3,4,5] {
        v = v + v * v
    }
    show( r )
    // shows 2 6 12 20 30
```

The `pow` array. Let's make an array that contains functions and they make a `pow` of those arguments. Just as an example

```swift
    let make_pows = fn( max ) {
        var res = [ ]               // result is an array
        for i in 0..max {           // array starts from 0
            res = res + [ fn( c ) { // add an element to the result
                if( i == 0 ) {
                    return 1
                }
                var res = 1         // internal `res`
                for j in 1..i {
                    res = res * c
                }
                res
            } ]
        }
        res     // return
    }
    let arr = make_pows( 10 )       // makes an array 0..10.
    io.puts( arr[0](1000) ) // `1`
    io.puts( arr[1](1000) ) // `1000`
    io.puts( arr[2](1000) ) // `1000000`
    io.puts( arr[10](2)   ) // `1024`

```
Five little monkeys

```swift

    let the_monkey_song = fn( monkeys ) {
        let sing = " little monkey jumping on the bed\n"
                    "She fell off and bumped her head\n"
                    "Momma called the doctor and the doctor said\n"
                    "\"No more monkeys jumping on the bed!\"\n"
        let plur = " little monkeys jumping on the bed\n"
                     "🐒 fell off and bumped his head\n"
                     "Momma called the doctor and the doctor said\n"
                     "\"No more monkeys jumping on the bed!\"\n"
        for i in monkeys..1, -1 {
            if ( i == 1 ) {
                io.puts( "🐒", sing )
            } else {
                io.puts( "🐒" * i, plur)
            }
        }
        return
    }
    the_monkey_song( 5 )

    //🐒🐒🐒🐒🐒 little monkeys jumping on the bed
    //One fell off and bumped his head
    //Momma called the doctor and the doctor said
    //"No more monkeys jumping on the bed!"
    //
    // .................................
    //
    //🐒🐒 little monkeys jumping on the bed
    //One fell off and bumped his head
    //Momma called the doctor and the doctor said
    //"No more monkeys jumping on the bed!"
    //
    //🐒 little monkey jumping on the bed
    //She fell off and bumped her head
    //Momma called the doctor and the doctor said
    //"No more monkeys jumping on the bed!"

```

#### break and continue keywords

Of course they exist. And they do what they do always.
They interrupt current loop and then `continue` makes the loop continue and `break` just breaks it (of course!).

```swift
    for i in [1,2,3,4,5,6,7,0,-1,-2,-3,-4] {
        if( i == 0 ) {
            break
        }
        io.put(i)
    }
    io.puts( )
    // 1234567

    for i in [1,2,3,4,5,6,7,0,-1,-2,-3,-4] {
        if( i == 0 ) {
            continue
        }
        io.put(i)
    }
    io.puts( )
    // 1234567-1-2-3-4
```

`continue` and `break` must be in the body of a loop.

```swift

    for i in 10 {
        if( i == 5 ) {
            if( true ) {
                if( true ) {
                    if( true ) { /// just an example
                        continue
                    }
                }
            }
        }
        io.put(i)
    }
    // 012346789

    let r = fn( ) {
        break
    }
    // 2:8 Unexpected 'break'

    for i in 10 {
        let r = fn( ) {
            continue
        }
        r( )
    }
    // 3:12 Unexpected 'continue'
```

### if elif else
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

### Operators
Mico has: +, -, *, /, %, ==, !=, <, >, <=, >=, |, &, ^, &&, ||, >>, <<, ~, in
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

Logical operators `&&` and `||` are lazy. `&&` doesn't eval its right side if its left side is `false`.
`||` does the same thing. It returns `true` if its left side is `true`

```swift
    let f = 10 > 100 && "100" < 100 // ok. `f` is `false`
    let t = 10 > 100 || "100" < 100 // failed
    // error: [1:26] Infix operation '<' is not defined for string and integer
```

#### Operator `in`
Checks if a value exists in a container or in an interval
```swift
    io.puts( 1 in { 1: "one", 2: "two", 3: "three" } )  // `true`
    io.puts( 4 in { 1: "one", 2: "two", 3: "three" } )  // `false`
    io.puts( 0 in 0..100 )                              // `true`
    io.puts( 7 in 0..6   )                              // `false`
    io.puts( 7 in [1,2,3,4,5,6]   )                     // `false`
    io.puts( 5 in [1,2,3,4,5,6]   )                     // `true`
```
Complexity
```swift
    i in a..b  //  O(1)
    i in {...} //  O(1) aprox. see `hash tables`
    i in [...] //  O(n)
```

## Functions
### First-class Citizen
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
### Partial application
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
### Variadic parameters
Functions in Mico can accept a variable number of arguments.
```swift
    let param_count = fn( ...parms ) { // prefix operator elipsis must be the last one
        return len(parms)
    }
    let seven = param_count( 1, 2, [], {}, fn(){ }, 6, 7 )
    let zero  = param_count( )
    io.puts("seven is ", seven, " and zero is ", zero)
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

### Pipe operator
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

### Tail Call Optimization
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
        io.puts("1 ", c)
        if( c > 0 ) {
            call2(c - 1) // indirect call
        } else {
            "Ok 1"
        }
    }
    let call2 = fn(c) {
        io.puts("2 ", c)
        if( c > 0 ) {
            call1(c - 1) // indirect call
        } else {
            "Ok 2"
        }
    }
    call1(0xFFFFFF) // Ok
```
## A Macro System For Monkey


Yes. Finally I've had some free time to add the system; https://interpreterbook.com/lost/
So. First of all **quote**, **unquote**, and **macro** are keywords and I can parse them as I wish.
And there are 2 types of **quote**

### Expression quote
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
### Statements quote
```swift

    /// statements quote = quote + { + statements + }

    let a = quote { let a = 1000; let b = a * 2 } // `a` is an ast node
                                                  // and contains 2 `let` statements
    unquote(a) // => eval `let a = 1000; let b = a * 2` => null
               // now `a` is 1000 and `b` is 2000
```
### macro

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
        io.puts("Here the macro inlined!")
        if(!(unquote(condition))) {
            unquote(consequence);
        } else {
            unquote(alternative);
        };
    }
    unless(10 > 5, io.puts("not greater"), io.puts("greater"))
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
    //  io.puts("Here the macro inlined!");
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
### Builtin Macroses
Yes, it is possible to add them. One test macro `__I` I've added to the macro processor.
It just returns an ident that contains a concatination of all of the parameters it has.
```js
    let __I(name, 1) = "this is the first one"

    io.puts(name1)
    io.puts(__I(name, 1)) // also should work
    /// this is the firts one

    io.puts(__I(name, 2))
    /// error: [1:29] Identifier not found 'name2'

    let __I(sum, 2) = fn(a,b) { a + b }
    __I(sum, 2)(1, 2) /// => sum2(1, 2) => 3
    sum2(1, 2) // => 3

```

## Modules

Modules are just pieces of the environment.
They are more like `namespaces` in c++ but in Mico modules are first-class citizens.

```swift

    let a = module {
        let value = "value a"
        let show = fn( ) { io.puts( value ) }
    }
    a.show( ) // shows `value a`
    let b = module {
        let a = a // shadowing
        let show = fn( ) { io.puts( "module b: " + a.value ) }
    }

    b.a.show( ) // same as a.show( )
    b.show( )   // `module b: value a`

    let show = fn( mod ) { // accepts module like a parameter
        mod.show( )
    }

    show( a ) /// `value a`
    show( b ) /// `module b: value a`

```

### Inheritance
A module can inherit values from another module. In the case of inheritance
all elements of the parent are available in the child. And by the child.
```swift
    let a = module {
        var value = "value a"
        let show = fn( ) { io.puts( value ) }
        let set = fn( val ) { value = val }
    }

    let b = module: a { } // Inheritance

    b.show( )           // `value a`
    b.set( "changed!" )
    io.puts(b.value)       // `changed!`
    a.show( )           // `changed!`
```
The inheritance list is a list of the expression. Every expression returns a module object. And if doesn't...
```swift
    let a = module: 1 + 1 { }
    // error: [1:18] Bad parent for module OBJ_INTEGER 2
```

A module can have one or more parents.

```swift
    let a = module {
        let value = "value a"
        let showa = fn( ) { io.puts( value ) }
        let seta = fn( val ) { value = val }
    }

    let b = module {
        let value = "value b"
        let showb = fn( ) { io.puts( value ) }
        let setb = fn( val ) { value = val }
    }

    let c = module: a, b { } // Inheritance

    c.showa( )      // `value a`
    c.showb( )      // `value b`
    io.puts(c.a.value) // `value a`
    io.puts(c.b.value) // `value b`

```
And what is here?
```swift
    io.puts(c.value)
```
Well here is `value a`. Because module `a` is the first in the inheritance tree.

### Anonymous

Anonymouses modules are also available. They are almoust useless because we can't obtain values from them right?
Wrong! It can be used for the inheritance.
```swift
    let new_module = fn( val ) {
        module {
            let val = val
            let get = fn( ) { val }
        }
    }
    let a = module a: new_module(100) { }
    let b = module b: new_module(200) { }
    io.puts(a.get( )) // `100`
    io.puts(b.get( )) // `200`
```
By the way we can add a name to a module without `let` expression
```swift
    let new_module0 = fn( val ) {
        module parent {
            let val = val
        }
    }

    let new_module1 = fn( val ) {
        let parent = module  {
            let val = val
        }
        parent
    }

    let a = module: new_module0(100) { }
    let b = module: new_module1(200) { }
    io.puts(a.parent.val) // `100`
    io.puts(b.parent.val) // `200`
```
Here `new_module0` and `new_module1` are almost equal. But the code
```swift
    module parent { let val = val }
```
Doesn't change the current environment. It just makes a module with name. And as far as the module doesn't have a name in the environment it can be used as an "anonymous" module...but with name. Yeah...

Also it's possible to chenge the module's name in the `let` expression.
```swift
    let a = module name {
        let val = 1000
    }
    let b = module: a { }
    io.puts(b.name.val) /// ok `1000`
    io.puts(b.a.value)  /// oops `error: [6:7] Identifier not found 'a'`
```

## Standart Library

### io

### string

### dbg

