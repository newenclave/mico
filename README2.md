# Mico

Monkey :monkey: the language interpreter implementation done with C++. https://interpreterbook.com/

![Mico REPL Terminal](repl.png)

## Requirements

* C++ compiler (c++11 or later)
* The book https://interpreterbook.com/. If you have not read it yet.

## Table of Contents

* [Compile](#compile)
* [Monkey and Mico](#Monkey-and-Mico)

### Compile

Mico is a header-only project. Well you need just compile `main.cpp`

```
 github λ git clone https://github.com/newenclave/mico.git
 github λ cd mico
 mico λ git submodule update --init etool
 mico λ g++ main.cpp -O2 -Iinclude -Ietool/include -o mico

```
Thats all.

### Monkey and Mico
"Mico" is an implementation but of course it has some difference.
For now I'm pretty sure that Mico can run the major part of Monkey's code.

