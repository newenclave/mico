#ifndef MICO_EXPRESSION_INFIX_H
#define MICO_EXPRESSION_INFIX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::INFIX>: public typed_expr<type::INFIX> {

        using this_type = impl<type::INFIX>;

    public:

        using uptr = std::unique_ptr<impl>;

        impl<type::INFIX>( tokens::type tt, node::uptr lft )
            :token_(tt)
            ,left_(std::move(lft))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "(" << left_->str( ) << token_
                << (right_ ? right_->str( ) : "null" ) << ")";
            return oss.str( );
        }

        void set_left( node::uptr lft )
        {
            left_ = std::move(lft);
        }

        void set_right( node::uptr rght )
        {
            right_ = std::move(rght);
        }

        node::uptr &left( )
        {
            return left_;
        }

        node::uptr &right( )
        {
            return right_;
        }

        const node::uptr &left( ) const
        {
            return left_;
        }

        const node::uptr &right( ) const
        {
            return right_;
        }


        tokens::type token( ) const
        {
            return token_;
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( left_, call );
            ast::node::apply_mutator( right_, call );
        }

        bool is_const( ) const override
        {
            return left_->is_const( ) && right_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type(token_, node::call_clone( left_ ) ) );
            res->right_ = node::call_clone( right_ );
            return ast::node::uptr( std::move( res ) );
        }

    private:
        tokens::type    token_;
        node::uptr      left_;
        node::uptr      right_;
    };

    using infix = impl<type::INFIX>;

}}}

#endif // INFIX_H
