#ifndef MICO_EXPRESSION_REGISTRY_H
#define MICO_EXPRESSION_REGISTRY_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::REGISTRY>: public typed_expr<type::REGISTRY> {

        using this_type = impl<type::REGISTRY>;
    public:

        using uptr = std::unique_ptr<this_type>;
        using value_type = std::uintptr_t;

        explicit
        impl<type::REGISTRY>( value_type val )
            :value_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << std::hex << "reg(" << value( ) << ")";
            return "null";
        }

        value_type value( ) const
        {
            return value_;
        }

        void mutate( mutator_type ) override
        {

        }

        bool is_const( ) const override
        {
            return false;
        }

        ast::node::uptr clone( ) const override
        {
            return ast::node::uptr( new this_type( value_ ) );
        }

    private:

        value_type value_;
    };

    using registry = impl<type::REGISTRY>;

}}}

#endif
