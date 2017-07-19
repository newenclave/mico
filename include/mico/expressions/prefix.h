#ifndef MICO_EXPRESSION_PREFIX_H
#define MICO_EXPRESSION_PREFIX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::PREFIX>: public typed_expr<type::PREFIX> {
    public:

        using uptr = std::unique_ptr<detail>;

        detail( tokens::type tt, expression::uptr exp )
            :token_(tt)
            ,expr_(std::move(exp))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "(" << token_ << expr_->str( ) << ")";
            return oss.str( );
        }

        expression *value( )
        {
            return expr_.get( );
        }

        tokens::type token( ) const
        {
            return token_;
        }

        void mutate( mutator_type call ) override
        {
            ast::expression::apply_mutator( expr_, call );
        }

    private:
        tokens::type     token_;
        expression::uptr expr_;
    };

}}}

#endif // PREFIX_H
