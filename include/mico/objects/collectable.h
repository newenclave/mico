#ifndef MICO_OBJECTS_COLLECTABLE_H
#define MICO_OBJECTS_COLLECTABLE_H

#include "mico/environment.h"
#include "mico/objects/base.h"

namespace mico { namespace objects {

    template <type TN>
    class collectable: public typed_base<TN> {

    public:

        explicit
        collectable( environment::sptr &e )
            :env_(e)
        {
            e->set_owner( this );
        }

        ~collectable( )
        { }

        environment::sptr env( )
        {
            auto l = env_.lock( );
            return l;
        }

        const environment::sptr env( ) const
        {
            auto l = env_.lock( );
            return l;
        }

        const environment *hold( ) const override
        {
            return env( ).get( );
        }

        bool mark_in( const environment *e ) override
        {
            return environment::mark_in( env( ), e );
        }

        bool unmark_in( const environment *e ) override
        {
            return environment::unmark_in( env( ), e );
        }

        std::size_t marked( ) const override
        {
            if( auto p = env( ) ) {
                return p->marked( );
            }
            return 0;
        }

    private:

        std::weak_ptr<environment>  env_;
    };

}}

#endif // COLLECTABLE_H
