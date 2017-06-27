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

struct test {
    test( ) = default;
    test( std::shared_ptr<test> p )
        :parent_(p)
    { }
    std::shared_ptr<test> parent_;
};

std::int64_t fib( std::int64_t x ) {
    //std::cout << x << "\n";
    if( x == 0 || x == 1 ) {
        return x;
    } else {
        return fib(x - 1) + fib( x - 2);
    }
}

enum class name {
    NONE = 0,
    STRING,
    INTEGER,
};

struct base {
    virtual ~base( ) = default;
    virtual void call( ) = 0;
};

template <name N>
struct derived;

template <>
struct derived<name::STRING>: public base {
    virtual void call( ) {
        std::cout << "String";
    }
};

template <>
struct derived<name::INTEGER>: public base {
    virtual void call( ) {
        std::cout << "INT";
    }
};

using str  = derived<name::STRING>;
using intg = derived<name::INTEGER>;

/*

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

*/

int main( )
{

    // let x = fn(count){if(count > 0) { rerurn x(count-1)} else {return 0}}


    //let fac = fn( x ) { if(x > 1) { x * fac(x - 1)  } else { 1 } }

    //let fib = fn(x) { if (x == 0) { 0 } elif(x==1) { 1 } else { fib(x - 1) + fib(x - 2); } };

//    std::cout << objects::obj_less( )( objects::unsigned_int::make(1),
//                                       objects::signed_int::make(10) );

//    std::cout << objects::obj_less( )( objects::signed_int::make(1),
//                                       objects::unsigned_int::make(10) );

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

    auto env = std::make_shared<enviroment>( );
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
