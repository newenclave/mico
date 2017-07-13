#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <sstream>
#include "mico/objects/base.h"

namespace mico { namespace objects {

    template <>
    class derived<type::BOOLEAN>: public typed_base<type::BOOLEAN> {
        using this_type = derived<type::BOOLEAN>;

        struct key { };

    public:

        using sptr = std::shared_ptr<this_type>;

        derived( bool v )
            :value_(v)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << (value_ ? "true" : "false");
            return oss.str( );
        }

        bool value( ) const
        {
            return value_;
        }

        static
        sptr make( bool val )
        {
            static auto true_this  = std::make_shared<this_type>( true );
            static auto false_this = std::make_shared<this_type>( false );
            return val ? true_this : false_this;
        }

        std::uint64_t hash( ) const override
        {
            return value_ ? 1 : 0;
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return o->value( ) == value( );
            }
            return false;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return make( value_ );
        }

    private:
        bool value_;
    };

}}

#endif // BOOLEAN_H
