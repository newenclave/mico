#ifndef MICO_STATE_H
#define MICO_STATE_H

#include <memory>
#include "mico/environment.h"

namespace mico {

    struct state {

        using sptr = std::shared_ptr<state>;

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

    private:
        environment::sptr env_;
    };
}

#endif // STATE_H
