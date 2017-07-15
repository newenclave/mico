#ifndef MICO_EXPRESSION_INDEX_H
#define MICO_EXPRESSION_INDEX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::INDEX>: public typed_expr<type::INDEX> {

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

    private:
        expression::uptr left_;
        expression::uptr expr_;
    };

}}}

#endif // INDEX_H
