#ifndef MICO_EXPRESSION_INDEX_H
#define MICO_EXPRESSION_INDEX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::INDEX>: public typed_expr<type::INDEX> {
        using this_type = impl<type::INDEX>;
    public:

        using uptr = std::unique_ptr<impl>;

        impl( node::uptr left, node::uptr param )
            :left_(std::move(left))
            ,expr_(std::move(param))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << value( )->str( ) << "[" << param( )->str( ) << "]";
            return oss.str( );
        }

        ast::node::uptr &value( )
        {
            return left_;
        }

        const ast::node::uptr &value( ) const
        {
            return left_;
        }

        ast::node::uptr &param( )
        {
            return expr_;
        }

        const ast::node::uptr &param( ) const
        {
            return expr_;
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( left_, call );
            ast::node::apply_mutator( expr_, call );
        }

        bool is_const( ) const override
        {
            return left_->is_const( ) && expr_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            auto l = node::call_clone( left_ );
            auto e = node::call_clone( expr_ );
            uptr res(new this_type( std::move( l ), std::move( e ) ) );
            return ast::node::uptr( std::move( res ) );
        }

    private:
        node::uptr left_;
        node::uptr expr_;
    };

    using index = impl<type::INDEX>;
}}}

#endif // INDEX_H
