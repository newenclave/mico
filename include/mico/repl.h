#ifndef MICO_REPL_H
#define MICO_REPL_H

#include <string>
#include <iostream>
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"

namespace mico {

    struct repl {


        static
        void run( )
        {
            while( true ) {
                std::string data;
                std::cout << ">>> ";
                std::getline( std::cin, data );
                auto prog = mico::parser::parse( data );
                if( prog.errors( ).empty( ) ) {
                    std::cout << prog.str( ) << "\n";
                    mico::eval::tree_walking tv;
                    std::cout << tv.eval( prog.states( )[0].get( ) )->str( )
                            << "\n";
                } else {
                    for( auto &e: prog.errors( ) ) {
                        std::cout << e << "\n";
                    }
                }
            }
        }
    };
}

#endif // REPL_H
