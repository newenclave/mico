#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"

int main( )
{
    std::string input = "0x545, 2000, 0b0101010, 0123234"
            ;

    auto tt = mico::lexer::make_token_list(input);
    return 0;
}
