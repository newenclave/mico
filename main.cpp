#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"
#include "mico/objects.h"
#include "mico/parser.h"

int main( )
{

    std::string input = //"let f0 = fn( ) { }\n"
                        "f0 ( 1, 2, 3 );\n"
                        "fn(x1, x2, x3) {\n"
                        "   x1 + 10;"
                        "   x2 + 30;"
                        "   return 100, 90, 1 + 2 * 3;"
                        "   x3"
                        "}(1, 2, 3)\n"
                        //"return 2 + 2 * 2;\n"
            ;

    auto pp = mico::parser::parse(input);

    std::cout << pp.str( ) << "\n";

    for( auto &e: pp.errors( ) ) {
        std::cout << "Error: " <<  e << "\n";
    }

    //return 0;

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
