#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"
#include "mico/objects.h"
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "mico/repl.h"

int run_repl( )
{
    mico::repl::run( );

    return 0;
}

using namespace mico;

/*

let fac = fn(val) {
    let impl = fn( val, acc ) {
        if val > 1 {
            impl( val - 1, acc * val )
        } else {
            acc
        }
    }
    impl( val, 1 )
}

let fib = fn( n ) {
    let x = fn(count) {
        if count > 0 {
            fn( ){ x(count - 1) }
        } else {
            fn( ){ return 15 }
        }
    }
    let impl = fn( a, b, n ) {
        if n > 0 {
            impl( b, a + b, n -1 )
        } else {
            x(100)( );
            a
        }
    }
    impl(0, 1, n)
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

let t = fn( x ) {
    let m = fn( y ) {
        y + 10
    }
    m( x + 1 )
}

let sum = fn( count ) { // start
    let impl = fn( a, new ) { /// internal
            r(10)
        if( new == 0 ) {
            a
        } else {
            return impl( a + 1, new - 1 )
        }
    }
    impl( 0, count )
}

let x = fn(count){if(count > 0) { return x(count - 1) } else {0} }
let x = fn(count){if(count > 0) { return x(count - 1) + 1; 60 } 0 }
let x = fn( ){ x( ) }

let fac = fn( x ) { if(x > 1) { x * fac(x - 1)  } else { 1 } }
let fib = fn(x) {
    if (x == 0) { 0 } elif ( x==1 ) { 1 }
    else { fib(x - 1) + fib(x - 2); }
};

*/

int main_lex( );

int main( )
{
    return main_lex( );

    return run_repl( );

    std::string input = "10.34e255;"
                        "if( \"test1\" < \"test1\" ) {"
                        "   true"
                        "} else {"
                        "  false "
                        "}"
//                        "if t > 10 {                \n"
//                        "   let x = 10;             \n"
//                        "   return 100;             \n"
//                        "} elif 1 < 0 * 0.12e3 {          \n"
//                        "   1 + 1 * 1               \n"
//                        "} else {                    \n"
//                        "   return \"string!!1\";   \n"
//                        "}\n"
//                        "1 * (0b0101 + 6)\n"
            ;

    auto pp = mico::parser::parse(input);
    std::cout << pp.str( ) << "\n";

    mico::eval::tree_walking tv;

    auto env = enviroment::make( );
    auto obj = tv.eval( &pp, env );

    std::cout << obj->str( ) << "\n";

    for( auto &e: pp.errors( ) ) {
        std::cout << "Error: " <<  e << "\n";
    }

    return 0;

    auto tt = mico::lexer::make(input);

    for( auto &l: tt ) {
        std::cout << l.where
                  << "\t" << l.ident.name
                  << "\t" << l.ident.literal
                  << "\n";
    }

    for( auto &e: tt.errors( ) ) {
        std::cout << "error: " << e << "\n";
    }

    return 0;
}
