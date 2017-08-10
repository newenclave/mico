#ifndef MICO_MODULES_GC_H
#define MICO_MODULES_GC_H

#include "mico/builtin.h"
#include "mico/builtin/common.h"
#include "mico/objects/module.h"

#include "mico/objects/slices.h"

#include "mico/charset/encoding.h"
#include "mico/environment.h"

namespace mico { namespace modules {

    struct gc {
        struct collect {

            collect( environment::sptr env )
                :root(env)
            { }

            objects::sptr operator ( )( objects::slist &pp, environment::sptr )
            {
                if( pp.empty( ) ) {
                    if( auto p = root.lock( ) ) {
                        p->get_state( ).GC( p );
                    }
                    return objects::integer::make( 0 );
                } else {
                    std::int64_t count = 0;
                    for( auto &o: pp ) {
                        switch ( o->get_type( ) ) {
                        case objects::type::FUNCTION: {
                            auto fun = objects::cast_func( o.get( ) );
                            fun->env( )->get_state( ).GC( fun->env( ) );
                            ++count;
                        }
                            break;
                        case objects::type::BUILTIN: {
                            auto fun = objects::cast_builtin( o.get( ) );
                            fun->env( )->get_state( ).GC( fun->env( ) );
                            ++count;
                        }
                            break;
                        case objects::type::MODULE: {
                            auto fun = objects::cast_mod( o.get( ) );
                            fun->env( )->get_state( ).GC( fun->env( ) );
                            ++count;
                        }
                            break;
                        }
                    }
                    return objects::integer::make( count );
                }
            }

            environment::wptr root;
        };

        static
        void load( environment::sptr &env, const std::string &name = "gc" )
        {
            using BC = builtin::common;
            auto mod_env = environment::make(env);
            auto mod = objects::module::make( mod_env, name );
            mod_env->set_const( "collect", BC::make( mod_env, collect(env) ) );
            env->set_const( name, mod );
        }
    };

}}

#endif // GC_H
