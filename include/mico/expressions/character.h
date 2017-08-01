#ifndef MICO_EXPRESSIONS_CHAR_H
#define MICO_EXPRESSIONS_CHAR_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

#include "mico/types.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::CHARACTER>: public typed_expr<type::CHARACTER> {

        using this_type = impl<type::CHARACTER>;

    public:

        using value_type = mico::string::value_type;
        using uptr = std::unique_ptr<this_type>;

        impl( value_type val )
            :value_(val)
        { }

        std::string str( ) const override
        {
            mico::string v( 1, value_ );
            return std::string("'") + charset::encoding::to_file(v) + "'";
        }

        value_type value( ) const
        {
            return value_;
        }

        static
        uptr make( value_type val )
        {
            return uptr( new this_type(val ) );
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
        value_type value_;
    };

    using character = impl<type::CHARACTER>;

}}}

#endif // STRING_H
