# mico

Monkey the language interpreter implementation done with C++. 
See https://interpreterbook.com/

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
#### tail call optimization (there is no such a thing in the book)

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


### ... are not supportet yet

#### arrays

```
let a = [1, 2, 3, 4, 5];
a[0] = 10;
```
#### built in functions

```
let a = [1, 2, 3, 4, 5];
let s = "Hello!"
let la = len(a) // 5 
let ls = len(s) // 6
```

#### comments

```
let x = 0 /// yeah, this piece of code will fail to compile because of comment 
```
