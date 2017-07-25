#ifndef MICO_BUILTIN_H
#define MICO_BUILTIN_H

#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "mico/state.h"
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
                std::cout << light << "Macroses:\n";
                for( auto &ss: s.macros( ).value( ) ) {
                    std::cout << cyan << ss.first << none
                              << " => "
                              << ss.second->str( )
                              << light << "\n==========\n" << none
                              ;
                }

                for( auto &ss: s.macros( ).built_in( ) ) {
                    std::cout << cyan << ss.first << none
                              << " => "
                              << ss.second->str( )
                              << light << "\n==========\n" << none
                              ;
                }
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

    template <bool NL>
    struct puts {

        objects::sptr operator ( )( objects::slist &pp, environment::sptr )
        {
            static const auto line = __LINE__;
            using objects::error;

            static auto res = objects::null::make( );

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
            if( NL ) {
                std::cout << std::endl;
            }
            return res;
        }
    };

    struct common_macro: public macro::processor::built_in_macro {
        using error_list = std::vector<std::string>;
        using call_type = std::function<ast::node::uptr ( ast::node *,
                                        macro::processor::scope *,
                                        ast::node_list &,
                                        error_list *)>;

        using uptr = std::unique_ptr<common_macro>;

        common_macro( call_type val )
            :call_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "macro(builtin@" << this << ")";
            return oss.str( );
        }

        void mutate( mutator_type /*call*/ ) override
        {
            return;
        }

        bool is_const( ) const override
        {
            return false;
        }

        static
        uptr make( call_type val )
        {
            return uptr(new common_macro( std::move(val ) ) );
        }

        ast::node::uptr call( ast::node *n, macro::processor::scope *s,
                              ast::node_list &params,
                              error_list *e) override
        {
            return call_(n, s,params,  e);
        }

        ast::node::uptr clone( ) const override
        {
            return make(call_);
        }

        call_type call_;
    };

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
    struct random_name {

        using eval_call = std::function<mico::objects::sptr(ast::node *)>;

        random_name( eval_call ec )
            :ev_(ec)
        { }

        ast::node::uptr operator ( )( ast::node * n,
                                      macro::processor::scope *  /*s*/,
                                      ast::node_list &p,
                                      common_macro::error_list * /*e*/ )
        {
            std::string res;
            for( auto &par: p ) {
                auto er = ev_(par.get( ));
                if( er->get_type( ) != objects::type::FAILURE ) {
                    auto pp = er->to_ast( n->pos( ) );
                    res += pp->str( );
                } else {
                    if( (par->get_type( ) == ast::type::IDENT) ||
                        (par->get_type( ) == ast::type::INTEGER ) )
                    res += par->str( );
                }
            }
            return ast::node::make<ast::expressions::ident>( n->pos( ),
                                                             std::move(res) );
        }
        eval_call ev_;
    };
#endif

    struct builtin {

        using eval_call = random_name::eval_call;

        static
        void init( mico::state &st, eval_call ev )
        {
            auto env = st.env( );
            env->set( "puts",       common::make( env, puts<true> { } ) );
            env->set( "put",        common::make( env, puts<false> { } ) );

            env->set( "len",        common::make( env, len { } ) );
            env->set( "copy",       common::make( env, copy { } ) );
            env->set( "__env",      common::make( env, env_show(env) ) );
            env->set( "__macro",    common::make( env, macro_show(env) ) );

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
            st.macros( ).set_built( "__I",
                                 common_macro::make( random_name(ev) ) );
            st.macros( ).set_built( "__concat_idents",
                                 common_macro::make( random_name(ev) ) );
#endif
        }
    };

}

#endif // BUILTIN_H
