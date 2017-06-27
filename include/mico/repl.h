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
            auto env = enviroment::make( );
            std::string data;
            std::cout << ">>> ";
            while( true ) {
                std::string tmp;
                std::getline( std::cin, tmp );
                if( tmp.empty( ) ) {
                    auto prog = mico::parser::parse( data );
                    if( prog.errors( ).empty( ) ) {
                        //std::cout << prog.str( ) << "\n";
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
                    data.clear( );
                    std::cout << ">>> ";
                } else if( data.empty( ) && tmp.size( )==1 && tmp[0] == 'q' ) {
                    break;
                } else {
                    data += " ";
                    data += tmp;
                    std::cout << "  > ";
                }
            }
            env->clear( );
        }
    };
}

#endif // REPL_H
