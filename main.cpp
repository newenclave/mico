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

int main( int argc, char * argv[ ]  )
{

    return run_repl( );
}

