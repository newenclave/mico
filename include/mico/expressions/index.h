#ifndef MICO_EXPRESSION_INDEX_H
#define MICO_EXPRESSION_INDEX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::INDEX>: public typed_expr<type::INDEX> {
        using this_type = detail<type::INDEX>;
    public:

        using uptr = std::unique_ptr<detail>;
        using param_type = expression::uptr;

        detail( expression::uptr left, expression::uptr param )
            :left_(std::move(left))
            ,expr_(std::move(param))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << value( )->str( ) << "[" << param( )->str( ) << "]";
            return oss.str( );
        }

        expression *value( )
        {
            return left_.get( );
        }

        const expression *value( ) const
        {
            return left_.get( );
        }

        expression *param( )
        {
            return expr_.get( );
        }

        const expression *param( ) const
        {
            return expr_.get( );
        }

        void mutate( mutator_type call ) override
        {
            ast::expression::apply_mutator( left_, call );
            ast::expression::apply_mutator( expr_, call );
        }

        bool is_const( ) const override
        {
            return left_->is_const( ) && expr_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            auto l = expression::call_clone( left_ );
            auto e = expression::call_clone( expr_ );
            uptr res(new this_type( std::move( l ), std::move( e ) ) );
            return res;
        }

    private:
        expression::uptr left_;
        expression::uptr expr_;
    };

}}}

#endif // INDEX_H
