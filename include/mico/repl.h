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
            auto env = std::make_shared<enviroment>( );
            while( true ) {
                std::string data;
                std::cout << ">>> ";
                std::getline( std::cin, data );
                auto prog = mico::parser::parse( data );
                if( prog.errors( ).empty( ) ) {
                    std::cout << prog.str( ) << "\n";
                    mico::eval::tree_walking tv;
                    if( prog.states( ).size( ) > 0 ) {
                        auto obj = tv.eval( &prog, env );
                        std::cout << obj->str( ) << "\n";
                    }
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
