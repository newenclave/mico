#ifndef MICO_STATE_H
#define MICO_STATE_H

#include <memory>
#include "mico/objects/base.h"
#include "mico/environment.h"

namespace mico {

    struct state {

        using sptr = std::shared_ptr<state>;
        using registry_type = std::map<std::uintptr_t, objects::sptr>;

        state(  )
        {
            env_ = environment::make( this );
        }

        std::uintptr_t add_registry_value( objects::sptr obj )
        {
            auto id = reinterpret_cast<std::uintptr_t>(obj.get( ));
            registry_[id] = obj;
            return id;
        }

        objects::sptr get_registry_value( std::uintptr_t id )
        {
            auto f = registry_.find( id );
            if( f != registry_.end( ) ) {
                return f->second;
            }
            return nullptr;
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

    private:
        bool              gc_deep_ = true;
        environment::sptr env_;
        registry_type     registry_;
    };
}

#endif // STATE_H
