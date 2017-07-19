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
        using node_type = ast::node;

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

        node *value( )
        {
            return expr_.get( );
        }

        tokens::type token( ) const
        {
            return token_;
        }

        ast::node::uptr reduce( ast::node::reduce_call call ) override
        {
            ast::node::call_reduce( expr_, call );
            return nullptr;
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(token_, expr_->clone( ) ));
        }

    private:
        tokens::type token_;
        node::uptr   expr_;
    };

}}}

#endif // PREFIX_H
