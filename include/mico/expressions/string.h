#ifndef MICO_EXPRESSIONS_STRING_H
#define MICO_EXPRESSIONS_STRING_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::STRING>: public typed_expr<type::STRING> {

        using this_type = impl<type::STRING>;
    public:
        using uptr = std::unique_ptr<this_type>;

        impl(std::string val)
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            return std::string("\"") + value_ + '"';
        }

        const std::string &value( ) const
        {
            return value_;
        }

        std::string &value( )
        {
            return value_;
        }

        void mutate( mutator_type /*call*/ ) override
        {

        }

        bool is_const( ) const override
        {
            return true;
        }

        ast::node::uptr clone( ) const override
        {
            return uptr( new this_type(value_ ) );
        }

    private:
        std::string value_;
    };

}}}

#endif // STRING_H
