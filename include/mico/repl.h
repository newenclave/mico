#ifndef MICO_REPL_H
#define MICO_REPL_H

#include <string>
#include <iostream>
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "mico/builtin.h"
#include "mico/objects.h"

namespace mico {


    struct repl {

        static
        void env_reset( enviroment::sptr env )
        {
            for( auto &r: env->data( ) ) {
                r.second->env_reset( );
            }
            for( auto &c: env->children( ) ) {
                env_reset( c );
            }
        }

        static
        void run( )
        {
            auto env = enviroment::make( );
            builtin::init( env );
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
                            if( obj->get_type( ) != objects::type::NULL_OBJ ) {
                                std::cout << obj->str( ) << "\n";
                            }
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
                    if( !data.empty( ) ) {
                        data += "\n";
                    }
                    data += tmp;
                    std::cout << "  > ";
                }
            }
            //env_reset( env );
        }
    };
}

#endif // REPL_H
