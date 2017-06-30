#ifndef MICO_REPL_H
#define MICO_REPL_H

#include <string>
#include <iostream>
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"

namespace mico {

class len_call: public objects::builtin {
    objects::sptr call( objects::slist &params, enviroment::sptr ) override
    {
        if( params.size( ) < 1 ) {
            return objects::error::make( "Not enough actual parameters for"
                                         " 'len'",
                                         tokens::position(__LINE__, 0));
        }
        auto dat = params[0];

        if( dat->get_type( ) == objects::type::STRING ) {
            auto s = static_cast<objects::string *>(dat.get( ));
            return objects::integer::make( s->value( ).size( ));
        } else if( dat->get_type( ) == objects::type::ARRAY ) {
            auto s = static_cast<objects::array *>(dat.get( ));
            return objects::integer::make( s->value( ).size( ));
        } else if( dat->get_type( ) == objects::type::TABLE ) {
            auto s = static_cast<objects::table *>(dat.get( ));
            return objects::integer::make( s->value( ).size( ));
        }
        return objects::error::make( "Invalid parameter for"
                                     " 'len'", tokens::position(__LINE__, 0));
    }

public:

    len_call( enviroment::sptr e )
        :objects::builtin(e)
    { }

    static
    objects::sptr make( enviroment::sptr e )
    {
        return std::make_shared<len_call>( e );
    }
};

    struct repl {

        static
        void run( )
        {
            auto env = enviroment::make( );
            env->set( "len", len_call::make( env ) );
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
                    if( !data.empty( ) ) {
                        data += "\n";
                    }
                    data += tmp;
                    std::cout << "  > ";
                }
            }
        }
    };
}

#endif // REPL_H
