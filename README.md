# mico

Monkey the language interpreter implementation done with C++. https://interpreterbook.com/

## Difference

"Mico" is an implementation but of course it has some difference.
It's so funny to add something new =)

*   **tail call optimization.**
    I hope Thorsten will add this thing to the book soon.

*   **line number for the tokens.**
    Yeap. Now I can see where the an error happens.
```
    error: [1:15] Identifier not found 'someUnknownCall'
```

*   **Floating point** numbers
```
    >>> let t = 0.0001          // t = 0.0001
    >>> let r = 1e100           // r = 1e+100
    >>> let test = .55 / .11    // test = 5
```

*   **Number formats**.
    It's possible add DEC, OCT, TER, BIN and HEX number
```
    >>> 123456         // decimal
    >>> 01234567       // oct 342391
    >>> 0b010101010101 // bin 1365
    >>> 0t02120120     // ternary 1878
    >>> 0xDEADBEEF     // hex 3735928559
```

*   **Number's gap symbol** `_`
```
    >>> 1_123_456          // decimal
    >>> 01_234_567         // oct 342391
    >>> 0b0101_0101_0101   // bin 1365
    >>> 0t2_120_120        // ternary 1878
    >>> 0xDEAD_BEEF        // hex 3735928559
    >>> 0.000_000_000_1    // float 1e-10
```

*   **Assign operator** `=`
```
    >>> let a = 0
    >>> a = 100             // a = 1000
    >>> b = 0               // error: [1:0] Identifier not found: 'b'
    >>> let m = [1, 2, 4]
    >>> m[2] = 3            // m = [1, 2, 3]
    >>> let m = {"one":1,
                 "two":2,
                 "three":4}
    >>> m["three"] = 3      // { "three":3, "two":2, "one":1 }

    // assign is a `right arm` operator.
    let a = [1,2,3,4,5]
    a[0] = a[1] = a[2] = a[3] = a[4] = 0
    // a = [0, 0, 0, 0, 0]

```

*   **Reference object**
    The object that contains an other object.
    Now I can change the values in arrays, tables (hashes) and the environment
```
    >>> m["three"] = 3 // m["three"] returns a reference object
                       // and then assign (the operation) changes the value
```
*   **elif** branch to **ifelse**
    Well...there is nothing to explain here =)
```
    let result = if( x < 0 ) {
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
*   **Hashes (i.e. tables) and Arrays** can be the keys for the other Hashes
```
    // should work!
    let point = fn( x, y, z ) { {"x": x, "y": y, "z": z} }
    let table = {
        point(0, 0, 1): 0.1,
        point(0, 1, 0): 0.2,
        point(0, 1, 1): 0.3,
        point(1, 0, 0): 0.4,
        point(1, 0, 1): 0.5,
        point(1, 1, 0): 0.6,
        point(1, 1, 1): 0.7,
    }
    // table[point(0, 1, 1)] // 0.3

    // Keys are immutable. So you cant change their values.
    let p = point(1, 100, 200)
    let t = {p: false} // here `p` is cloned
    p["x"] = -1
    t // is still has an unique key {"x": 1, "y": 100, "z": 200}
```

Is not yet complete.

### ... have been done

#### variables
```
// Bind values to names with let-statements
let version = 1;
let name = "Monkey programming language";

// Use expressions to produce values
let awesomeValue = (10 / 2) * 5 + 30;
let arrayWithValues = [1 + 1, 2 * 2, 3];
```

#### functions
```
// Define a `fibonacci` function
let fibonacci = fn(x) {
  if (x == 0) {
    0                // Monkey supports implicit returning of values
  } else {
    if (x == 1) {
      return 1;      // ... and explicit return statements
    } else {
      fibonacci(x - 1) + fibonacci(x - 2); // Recursion! Yay!
    }
  }
};
```
#### tail call optimization (there is no such a thing in the book yet)

```
// Define a `fibonacci` function with tail calls
let fibonacci = fn( n ) {
    let impl = fn( a, b, n ) {
        if(n > 0) {
            return impl( b, a + b, n -1 );
        } else {
            a
        }
    }
    impl(0, 1, n)
}

let forever = fn( ){ forever( ) } /// will spin forever

```

#### closures

```
// newGreeter returns a new function, that greets a `name` with the given
// `greeting`.
let newGreeter = fn(greeting) {
  // `puts` is a built-in function we add to the interpreter
  return fn(name) { puts(greeting + " " + name); }
};

// `hello` is a greeter function that says "Hello"
let hello = newGreeter("Hello");

// Calling it outputs the greeting:
hello("dear, future Reader!"); // => Hello dear, future Reader!
```

##### Closures keep their enviroment

```
let create_array = fn( ) {
    let a = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
    fn( ) { a } // returns a closure that keeps array `a`
}

let array = create_array( ) // get closure
let test = create_array( )  // get another closure

/// set all elemnts to 0.001

array( )[0] = array( )[1] = array( )[2] = array( )[3] = array( )[4] =
array( )[5] = array( )[6] = array( )[7] = array( )[8] = array( )[9] = 0.001

array( ) // yeah. it works and returns the array `a` with
// [0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001]

test( ) // returns [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

```

#### if ... elif ... else

```
/// it's possible to use 'if' like an expression

let detect = fn( x ) {
  let result =  if( x < 0 ) {
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
  return result
}
```

#### arrays

```
let a = [1, 2, 3, 4, 5];
a[0] = 10; // a = [10, 2, 3, 4, 5]
let x = a[0] + a[3] // x = 14
```

#### hashes

```
let h = {"string": 0, true: "true", 10: "int", 0.1: [0, 1]}
let calls = {true: fn(x){ x * x }, false: fn(x){ x + x } }
let test = calls[h[true] == "true"](10) /// tets == 100

/// hashes can be keys in other hashes

let point = fn( x, y, z ) { {"x": x, "y": y, "z": z} }

let table = {
    point(0, 0, 1): 0.1,
    point(0, 1, 0): 0.2,
    point(0, 1, 1): 0.3,
    point(1, 0, 0): 0.4,
    point(1, 0, 1): 0.5,
    point(1, 1, 0): 0.6,
    point(1, 1, 1): 0.7,
}

// table = { { "x":0, "y":0, "z":1 }:0.1,
//           { "x":0, "y":1, "z":0 }:0.2,
//           { "x":0, "y":1, "z":1 }:0.3
//           { "x":1, "y":0, "z":0 }:0.4,
//           { "x":1, "y":0, "z":1 }:0.5,
//           { "x":1, "y":1, "z":0 }:0.6,
//           { "x":1, "y":1, "z":1 }:0.7,
//         }

```

#### comments
```
let x = 0 // comment; comment starts with // sequence
```

#### built in functions

```
let a = [1, 2, 3, 4, 5];
let s = "Hello!"
let la = len(a) // 5
let ls = len(s) // 6
```

### ... are not supported yet

#### block comments
```
/*
This is a block of comments
*/
let a = 0
```

#### A Macro System For Monkey

https://interpreterbook.com/lost/

Nope. I have not started to implement it.
