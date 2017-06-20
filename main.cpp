#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"

int main( )
{
    std::string input = "0t2121002102 = ; \n 0xFF445DD00"
                        " : !\n0b0101010, 0123234\n"
                        "let x = 543;\n"
                        "false != true"
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
