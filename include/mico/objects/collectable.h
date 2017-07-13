#ifndef MICO_OBJECTS_COLLECTABLE_H
#define MICO_OBJECTS_COLLECTABLE_H

#include "mico/environment.h"
#include "mico/objects/base.h"

namespace mico { namespace objects {

    template <type TN>
    class collectable: public typed_base<TN> {

    public:

        collectable( std::shared_ptr<environment> e )
            :env_(e)
        { }

        ~collectable( )
        { }

        std::shared_ptr<environment> env( )
        {
            auto l = env_.lock( );
            return l;
        }

        const std::shared_ptr<environment> env( ) const
        {
            auto l = env_.lock( );
            return l;
        }

        bool lock_in( const environment *e ) override
        {
            return environment::lock_in( env( ), e );
        }

        bool unlock_in( const environment *e ) override
        {
            return environment::unlock_in( env( ), e );
        }

        std::size_t locked( ) const override
        {
            if( auto p = env( ) ) {
                return p->locked( );
            }
            return 0;
        }

    private:

        std::weak_ptr<environment>  env_;
    };

}}

#endif // COLLECTABLE_H
