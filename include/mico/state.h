#ifndef MICO_STATE_H
#define MICO_STATE_H

#include <memory>
#include <map>
#include "mico/objects/base.h"
#include "mico/environment.h"

namespace mico {

    struct state {

        using sptr         = std::shared_ptr<state>;
        using reg_key      = std::uintptr_t;
        using registry_map = std::map<std::uintptr_t, objects::sptr>;

        static
        reg_key to_regkey( objects::sptr obj )
        {
            return reinterpret_cast<reg_key>(obj.get( ));
        }

        state(  )
        {
            env_ = environment::make( this );
        }

        environment::sptr env( )
        {
            return env_;
        }

        const environment::sptr env( ) const
        {
            return env_;
        }

        void GC( environment::sptr where )
        {
            where->run_GC( gc_deep_ );
        }

        reg_key add_reg_value( objects::sptr val )
        {
            auto id = to_regkey( val );
            registry_[id] = val;
            return id;
        }

        objects::sptr get_reg_value( reg_key id )
        {
            auto f = registry_.find( id );
            if( f != registry_.end( ) ) {
                return f->second;
            }
            return nullptr;
        }

    private:

        bool              gc_deep_ = true;
        environment::sptr env_;
        registry_map      registry_;
    };
}

#endif // STATE_H
