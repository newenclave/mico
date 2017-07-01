#ifndef MICO_BUILTIN_H
#define MICO_BUILTIN_H

#include "mico/parser.h"
#include "mico/eval/tree_walking.h"

namespace mico {

    class common: public objects::builtin {

        static
        void def_init( enviroment::sptr )
        { }

        objects::sptr call( objects::slist &params,
                            enviroment::sptr e) override
        {
            return call_( params, e );
        }

        void init( enviroment::sptr e ) override
        {
            return init_( e );
        }

    public:

        using call_type = std::function< objects::sptr
                                        (objects::slist &, enviroment::sptr)>;
        using init_type = std::function<void (enviroment::sptr)>;

        common( enviroment::sptr e, call_type c )
            :objects::builtin(e)
            ,init_(&common::def_init)
            ,call_(std::move(c))
        { }

        common( enviroment::sptr e, init_type i, call_type c )
            :objects::builtin(e)
            ,init_(i ? std::move(i) : &common::def_init)
            ,call_(std::move(c))
        { }

        static
        objects::sptr make( enviroment::sptr e, call_type c )
        {
            return std::make_shared<common>( e, std::move(c) );
        }

        static
        objects::sptr make( enviroment::sptr e, init_type i, call_type c )
        {
            return std::make_shared<common>( e, std::move(i), std::move(c) );
        }

    private:
        init_type init_;
        call_type call_;
    };

    struct len {
        objects::sptr operator ( )( objects::slist &params, enviroment::sptr )
        {
            static const auto line = __LINE__;
            using objects::error;
            using objects::integer;

            if( params.size( ) < 1 ) {
                return error::make( tokens::position(line, 0),
                                    "Not enough actual parameters for 'len'");
            }

            auto dat = params[0];

            if( dat->get_type( ) == objects::type::STRING ) {
                auto s = static_cast<objects::string *>(dat.get( ));
                return integer::make( s->value( ).size( ));
            } else if( dat->get_type( ) == objects::type::ARRAY ) {
                auto s = static_cast<objects::array *>(dat.get( ));
                return integer::make( s->value( ).size( ));
            } else if( dat->get_type( ) == objects::type::TABLE ) {
                auto s = static_cast<objects::table *>(dat.get( ));
                return integer::make( s->value( ).size( ));
            }
            return error::make( tokens::position( line, 0),
                                "Invalid parameter for 'len'");
        }
    };

    struct builtin {
        static
        void init( enviroment::sptr env )
        {
            env->set( "len", common::make( env, len { } ) );
        }
    };

}

#endif // BUILTIN_H
