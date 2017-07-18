#ifndef MICO_STATEMENTS_H
#define MICO_STATEMENTS_H

#include "mico/ast.h"
//#include "mico/expressions.h"

namespace mico { namespace ast { namespace statements {

    template <type>
    class detail;

    template <>
    class detail<type::LET>: public typed_stmt<type::LET> {

    public:
        using uptr = std::unique_ptr<detail>;

        using expr_type        = expression::uptr;
        using ident_type       = expression::uptr;

        detail( expression::uptr id, expression::uptr val )
            :ident_(std::move(id))
            ,value_(std::move(val))
        { }

        std::string str( ) const
        {
            std::ostringstream oss;
            oss << "let " << ident( )->str( ) << " = " << value( )->str( );
            return oss.str( );
        }

        const expression *ident( ) const
        {
            return ident_.get( );
        }

        expression *ident( )
        {
            return ident_.get( );
        }


        const expression *value( ) const
        {
            return value_.get( );
        }

        expression *value( )
        {
            return value_.get( );
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new detail(expression::clone_call( ident_ ),
                                   expression::clone_call( value_ ) ) );
        }

    private:

        expression::uptr ident_;
        expression::uptr value_;
    };

    template <>
    class detail<type::RETURN>: public typed_stmt<type::RETURN> {

    public:
        using uptr = std::unique_ptr<detail>;

        detail( expression::uptr val )
            :value_(std::move(val))
        { }

        std::string str( ) const
        {
            std::ostringstream oss;
            oss << "return " << value( )->str( );
            return oss.str( );
        }

        const expression *value( ) const
        {
            return value_.get( );
        }

        expression *value( )
        {
            return value_.get( );
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new detail(expression::clone_call( value_ ) ) );
        }

    private:
        expression::uptr value_;
    };

    template <>
    class detail<type::EXPR>: public typed_stmt<type::EXPR> {
    public:
        using uptr = std::unique_ptr<detail>;

        detail( expression::uptr val )
            :expr_(std::move(val))
        { }

        std::string str( ) const
        {
            return expr_->str( );
        }

        expression::uptr &value( )
        {
            return expr_;
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new detail(expression::clone_call( expr_ ) ) );
        }

    private:
        expression::uptr expr_;
    };

    using let   = detail<type::LET>;
    using ret   = detail<type::RETURN>;
    using expr  = detail<type::EXPR>;

}}}

#endif // STATEMENTS_H
