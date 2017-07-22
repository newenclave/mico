#ifndef MICO_BUILTIN_H
#define MICO_BUILTIN_H

#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "etool/console/colors.h"

namespace mico {

    class common: public objects::builtin {

        static
        void def_init( environment::sptr )
        { }

        objects::sptr call( objects::slist &params,
                            environment::sptr e) override
        {
            return call_( params, e );
        }

        void init( environment::sptr e ) override
        {
            return init_( e );
        }

    public:

        using call_type = std::function< objects::sptr
                                        (objects::slist &, environment::sptr)>;
        using init_type = std::function<void (environment::sptr)>;

        common( environment::sptr e, call_type c )
            :objects::builtin(e)
            ,init_(&common::def_init)
            ,call_(std::move(c))
        { }

        common( environment::sptr e, init_type i, call_type c )
            :objects::builtin(e)
            ,init_(i ? std::move(i) : &common::def_init)
            ,call_(std::move(c))
        { }

        std::uintptr_t stub_number( ) const override
        {
            return reinterpret_cast<std::uintptr_t>(this);
        }

        static
        objects::sptr make( environment::sptr e, call_type c )
        {
            return std::make_shared<common>( e, std::move(c) );
        }

        static
        objects::sptr make( environment::sptr e, init_type i, call_type c )
        {
            return std::make_shared<common>( e, std::move(i), std::move(c) );
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<common>( env( ), init_, call_ );
        }

    private:
        init_type init_;
        call_type call_;
    };

    struct len {
        objects::sptr operator ( )( objects::slist &params, environment::sptr )
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
                auto s = objects::cast_string(dat.get( ));
                return integer::make( s->value( ).size( ));
            } else if( dat->get_type( ) == objects::type::ARRAY ) {
                auto s = objects::cast_array(dat.get( ));
                return integer::make( s->value( ).size( ));
            } else if( dat->get_type( ) == objects::type::TABLE ) {
                auto s = objects::cast_table(dat.get( ));
                return integer::make( s->value( ).size( ));
            }
            return error::make( tokens::position( line, 0),
                                "Invalid parameter for 'len'");
        }
    };

    struct env_show {
        env_show( environment::sptr e )
            :env(e)
        { }

        objects::sptr operator ( )( objects::slist &, environment::sptr )
        {
            if( auto l = env.lock( ) ) {
                l->introspect(  );
            }
            return objects::null::make( );
        }
        environment::wptr env;
    };

    struct macro_show {

        macro_show( environment::sptr e )
            :env(e)
        { }

        objects::sptr operator ( )( objects::slist &, environment::sptr )
        {
            using namespace etool::console::ccout;
            if( auto l = env.lock( ) ) {
                auto &s(l->get_state( ));
                std::cout << light << "Macros:\n";
                for( auto &ss: s.macros( ).value( ) ) {
                    std::cout << cyan << ss.first << none
                              << " => "
                              << ss.second->str( )
                              << light << "\n==========\n" << none
                              ;
                }
                std::cout << light << "End of macros.\n" << none;
            }
            return objects::null::make( );
        }
        environment::wptr env;
    };

    struct copy {
        objects::sptr operator ( )( objects::slist &p, environment::sptr )
        {
            if( p.size( ) ) {
                return p[0]->clone( );
            }
            return objects::null::make( );
        }
    };

    struct puts {
        objects::sptr operator ( )( objects::slist &pp, environment::sptr )
        {
            static const auto line = __LINE__;
            using objects::error;

            std::size_t count = 0;
            for( auto &p: pp ) {
                ++count;
                if( p->get_type( ) == objects::type::STRING ) {
                    auto s = objects::cast_string(p.get( ));
                    std::cout << s->value( );
                } else if(p->get_type( ) == objects::type::INTEGER ) {
                    auto s = objects::cast_int(p.get( ));
                    std::cout << s->value( );
                } else if(p->get_type( ) == objects::type::FLOAT ) {
                    auto s = objects::cast_float(p.get( ));
                    std::cout << s->value( );
                } else if(p->get_type( ) == objects::type::BOOLEAN ) {
                    auto s = objects::cast_bool(p.get( ));
                    std::cout << std::boolalpha << s->value( );
                } else {
                    return error::make( tokens::position( line, 0),
                                        "Invalid parameter ", count,
                                        " for 'puts': ", p->get_type( ));
                }
            }
            std::cout << std::endl;
            return objects::null::make( );
        }
    };

    struct builtin {
        static
        void init( environment::sptr env )
        {
            env->set( "len",        common::make( env, len { } ) );
            env->set( "puts",       common::make( env, puts { } ) );
            env->set( "copy",       common::make( env, copy { } ) );
            env->set( "__env",      common::make( env, env_show(env) ) );
            env->set( "__macro",    common::make( env, macro_show(env) ) );
        }
    };

}

#endif // BUILTIN_H
