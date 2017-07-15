#ifndef MICO_EXPRESSION_INFIX_H
#define MICO_EXPRESSION_INFIX_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::INFIX>: public typed_expr<type::INFIX> {
    public:

        using uptr = std::unique_ptr<detail>;

        detail( tokens::type tt, expression::uptr lft )
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

        void set_left( expression::uptr lft )
        {
            left_ = std::move(lft);
        }

        void set_right( expression::uptr rght )
        {
            right_ = std::move(rght);
        }

        expression::uptr &left( )
        {
            return left_;
        }

        expression::uptr &right( )
        {
            return right_;
        }

        const expression::uptr &left( ) const
        {
            return left_;
        }

        const expression::uptr &right( ) const
        {
            return right_;
        }


        tokens::type token( ) const
        {
            return token_;
        }

    private:
        tokens::type     token_;
        expression::uptr left_;
        expression::uptr right_;
    };

}}}

#endif // INFIX_H
