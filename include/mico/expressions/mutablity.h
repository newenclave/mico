#ifndef MICO_EXPRESSION_MUTABILITY_H
#define MICO_EXPRESSION_MUTABILITY_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <type TN, bool Val>
    class mut_impl: public typed_expr<TN> {

        using this_type = mut_impl<TN, Val>;
    public:

        using uptr = std::unique_ptr<this_type>;

        explicit
        mut_impl<TN, Val>( ast::node::uptr val )
            :value_(std::move(val))
        { }

        bool is_mutable( ) const
        {
            return Val;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "quote(" << value_->str( ) << ")";
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

        void mutate( node::mutator_type call ) override
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
            return Val;
        }

    private:
        ast::node::uptr value_;
    };

    using mod_mut   = mut_impl<type::MOD_MUT,   true>;
    using mod_const = mut_impl<type::MOD_CONST, false>;

}}}

#endif // QUOTE_H
