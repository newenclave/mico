#ifndef MICO_EXPRESSION_PREFIX_H
#define MICO_EXPRESSION_PREFIX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::PREFIX>: public typed_expr<type::PREFIX> {
        using this_type = detail<type::PREFIX>;
    public:

        using uptr = std::unique_ptr<detail>;

        detail( tokens::type tt, node::uptr exp )
            :token_(tt)
            ,expr_(std::move(exp))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "(" << token_ << expr_->str( ) << ")";
            return oss.str( );
        }

        node::uptr &value( )
        {
            return expr_;
        }

        const node::uptr &value( ) const
        {
            return expr_;
        }

        tokens::type token( ) const
        {
            return token_;
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( expr_, call );
        }

        bool is_const( ) const override
        {
            return expr_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(token_, node::call_clone(expr_) ) );
        }

    private:
        tokens::type token_;
        node::uptr   expr_;
    };

}}}

#endif // PREFIX_H
