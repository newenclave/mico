#ifndef MICO_EXPRESSION_CALL_H
#define MICO_EXPRESSION_CALL_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::CALL>: public typed_expr<type::CALL> {
        using this_type = impl<type::CALL>;
    public:

        using list_type  = expressions::impl<ast::type::LIST>;
        using uptr       = std::unique_ptr<impl>;
        using param_type = list_type::uptr;

        impl( node::uptr f )
            :expr_(std::move(f))
            ,params_(list_type::make_params( ))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << expr_->str( ) << "(";
            oss << params_->str( );
            oss << ")";
            return oss.str( );
        }

        const param_type &params( ) const
        {
            return params_;
        }

        ast::node_list &param_list( )
        {
            return params_->value( );
        }

        const ast::node_list &param_list( ) const
        {
            return params_->value( );
        }

        ast::node::uptr &param_at( std::size_t pos )
        {
            return params_->value( )[pos];
        }

        const ast::node::uptr &param_at( std::size_t pos ) const
        {
            return params_->value( )[pos];
        }

        void set_params( param_type val )
        {
            params_ = std::move(val);
        }

        node::uptr &func( )
        {
            return expr_;
        }

        const node::uptr &func( ) const
        {
            return expr_;
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( expr_, call );
            for( auto &a: params_->value( ) ) {
                ast::node::apply_mutator( a, call );
            }
        }

        bool is_const( ) const override
        {
            return expr_->is_const( ) && params( )->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            auto expr = node::call_clone( expr_ );
            uptr res(new this_type( std::move( expr ) ) );
            res->params_ = params_->clone_me( );
            return res;
        }

    private:
        node::uptr  expr_;
        param_type  params_;
    };


}}}

#endif // CALL_H
