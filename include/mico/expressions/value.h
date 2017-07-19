#ifndef MICO_EXPRESSIONS_VALUE_H
#define MICO_EXPRESSIONS_VALUE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <type TN, typename ValueT>
    class value_expr: public typed_expr<TN> {

    public:

        using uptr       = std::unique_ptr<value_expr>;
        using value_type = ValueT;

        value_expr( value_type val )
            :value_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << std::boolalpha << value_;
            return oss.str( );
        }

        value_type value( ) const
        {
            return value_;
        }

        void mutate( ast::node::mutator_type ) override
        {

        }

        bool is_const( ) const override
        {
            return true;
        }

    private:
        value_type value_;
    };

    template <>
    class impl<type::INTEGER>:
            public value_expr<type::INTEGER, std::int64_t> {
        using this_type = impl<type::INTEGER>;
    public:

        using uptr = std::unique_ptr<this_type>;

        impl( std::int64_t val )
            :value_expr(val)
        { }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(value( ) ) );
        }
    };

    template <>
    class impl<type::FLOAT>:
            public value_expr<type::FLOAT, double> {
        using this_type = impl<type::FLOAT>;
    public:

        using uptr = std::unique_ptr<this_type>;

        impl( double val )
            :value_expr(val)
        { }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(value( ) ) );
        }

    };

    template <>
    class impl<type::BOOLEAN>:
            public value_expr<type::BOOLEAN, bool> {
        using this_type = impl<type::BOOLEAN>;
    public:

        using uptr = std::unique_ptr<this_type>;

        impl( bool val )
            :value_expr(val)
        { }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(value( ) ) );
        }
    };

}}}

#endif // VALUE_H
