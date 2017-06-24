#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"
#include "mico/objects.h"
#include "mico/parser.h"

int main( )
{

    std::string input = "if t > 10 {                \n"
                        "   let x = 10;             \n"
                        "   return 100;             \n"
                        "} elif 1 < 0 * 0.12e3 {          \n"
                        "   1 + 1 * 1               \n"
                        "} else {                   \n"
                        "   return \"string!!1\";   \n"
                        "}\n"
                        "1 * (2 + 6)\n"
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
