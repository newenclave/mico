#ifndef MICO_EXPRESSIONS_IDENT_H
#define MICO_EXPRESSIONS_IDENT_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::IDENT>: public typed_expr<type::IDENT> {

        using this_type = impl<type::IDENT>;

    public:

        using uptr = std::unique_ptr<this_type>;

        impl(std::string val)
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            return value_;
        }

        const std::string &value( ) const
        {
            return value_;
        }

        void mutate( mutator_type /*call*/ ) override
        {
            /// hm...
        }

        bool is_const( ) const override
        {
            return false;
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(value_));
        }

    private:
        std::string value_;
    };

}}}

#endif // IDENT_H
