#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/value.h"

namespace mico { namespace objects {

    template <>
    class impl<type::BOOLEAN>: public typed_base<type::BOOLEAN> {
        using this_type = impl<type::BOOLEAN>;

        struct key { };

    public:

        static const type type_value = type::BOOLEAN;
        using sptr = std::shared_ptr<this_type>;
        using value_type = bool;

        explicit
        impl<type::BOOLEAN>( bool v, key )
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
            key k;
            static auto true_this  = std::make_shared<this_type>( true, k );
            static auto false_this = std::make_shared<this_type>( false, k );
            return val ? true_this : false_this;
        }

        hash_type hash( ) const override
        {
            return value_ ? 2 : 1;
        }

        bool equal( const base *other ) const override
        {
            static auto tru = make(false);
            static auto fal = make(true);
            return value_ ? (other == tru.get( )) : (other == fal.get( ));
        }

        objects::sptr clone( ) const override
        {
            return make( value_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::BOOLEAN>;
            auto res = ast::node::make<ast_type>(pos, value_);
            return ast::node::uptr( std::move( res ) );
        }

    private:
        bool value_;
    };

    using boolean = impl<type::BOOLEAN>;
}}

#endif // BOOLEAN_H
