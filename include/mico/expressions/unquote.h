#ifndef MICO_EXPRESSION_UNQUOTE_H
#define MICO_EXPRESSION_UNQUOTE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::UNQUOTE>: public typed_expr<type::UNQUOTE> {

        using this_type = impl<type::UNQUOTE>;
    public:

        using uptr = std::unique_ptr<this_type>;

        impl<type::UNQUOTE>( ast::node::uptr val )
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "unquote(" << value_->str( ) << ")";
            return oss.str( );
        }

        ast::node::uptr &value( )
        {
            return value_;
        }

        const ast::node::uptr &value( ) const
        {
            return value_;
        }

        static
        uptr make( ast::node::uptr val )
        {
            return uptr(new this_type( std::move( val ) ) );
        }

        void mutate( mutator_type call ) override
        {
            //// hm
            ast::node::apply_mutator( value_, call );
        }

        ast::node::uptr clone( ) const override
        {
            return make( value_->clone( ) );
        }

        bool is_const( ) const override
        {
            return false;
        }


    private:
        ast::node::uptr value_;
    };

}}}

#endif // !defined(DISABLE_MACRO)
#endif // QUOTE_H
