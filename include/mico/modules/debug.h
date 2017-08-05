#ifndef MICO_MODULES_DEBUG_H
#define MICO_MODULES_DEBUG_H

#include "mico/builtin.h"
#include "mico/builtin/common.h"
#include "mico/objects/module.h"

namespace mico { namespace modules {

    struct env_show {

        explicit
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

    struct debug {
        static
        void load( environment::sptr &env, const std::string &name = "dbg" )
        {
            using BC = builtin::common;
            auto mod_e = environment::make(env);
            auto mod = objects::module::make( mod_e, name );
            mod_e->set_const( "env", BC::make( mod_e, env_show(env) ) );
            env->set_const( name, mod );
        }
    };
}}

#endif // DEBUG_H
