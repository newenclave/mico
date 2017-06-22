#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"
#include "mico/objects.h"
#include "mico/parser.h"

int main( )
{

    std::string input = "let x = !-009090.7887;  \n"
                        "let z = \"sdf\";         \n"
                        "let r = z - 0.05 + !10; \n"
                        "let test_str = 2 + 2 * 2;\n"
                        "let t = op\n"
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
