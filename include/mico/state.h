#ifndef MICO_STATE_H
#define MICO_STATE_H

#include <memory>
#include "mico/objects/base.h"
#include "mico/environment.h"
#include "mico/macro/processor.h"

namespace mico {

    struct state {

        using sptr          = std::shared_ptr<state>;
        using registry_type = std::map<std::uintptr_t, objects::sptr>;
#if !defined(DISABLE_MACRO)
        using macro_scope   = macro::processor::scope;
#endif
        state(  )
        {
            env_ = environment::make( this );
        }

        std::uintptr_t add_registry_value( std::uintptr_t id,
                                           objects::sptr obj )
        {
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

#if !defined(DISABLE_MACRO)
        macro_scope &macros( )
        {
            return macro_;
        }

        const macro_scope &macros( ) const
        {
            return macro_;
        }
#endif

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

#if !defined(DISABLE_MACRO)
        macro_scope       macro_;
#endif
    };
}

#endif // STATE_H
