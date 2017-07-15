#include <iostream>

#include "etool/slices/memory.h"

#include "mico/tokens.h"
#include "mico/lexer.h"

#include "mico/objects.h"
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "mico/repl.h"

#include <stdio.h>

int run_repl( )
{
    mico::repl::run( );

    return 0;
}

using namespace mico;

void run_file( )
{

}

int main_lex( );

union test {
    char bug[1024];
    void *ptr;
};

int main( int argc, char * argv[ ]  )
{
    test tt;
    tt.bug[0] = 0;
    tt.bug[1] = 0;
    tt.bug[2] = 0;
    tt.bug[3] = 0;
    tt.bug[4] = 0;
    tt.bug[5] = 0;
    tt.bug[6] = 0;
    tt.bug[7] = 0;

    std::cout << tt.ptr << "\n";

    return run_repl( );
}

