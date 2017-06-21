#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"

int main( )
{

    std::string input = "let x = 0.1;             \n"
                        "let z = 0b01010;         \n"
                        "let r = z * (0.05 + 10); \n"
            ;

    auto tt = mico::lexer::make(input);

    for( auto &l: tt ) {
        std::cout << l.line << "\t" << l.pos
                  << "\t" << l.ident.name
                  << "\t" << l.ident.literal
                  << "\n";
    }

    for( auto &e: tt.errors( ) ) {
        std::cout << "error: " << e << "\n";
    }

    return 0;
}
