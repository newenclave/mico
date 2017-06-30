#ifndef MICO_BUILTIN_H
#define MICO_BUILTIN_H

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

    struct builtin {
        static
        void init( enviroment::sptr env )
        {
            env->set( "len", len_call::make( env ) );
        }
    };

}

#endif // BUILTIN_H
