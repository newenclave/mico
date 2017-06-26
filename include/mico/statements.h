#ifndef MICO_STATEMENTS_H
#define MICO_STATEMENTS_H

#include "mico/ast.h"
#include "mico/expressions.h"

namespace mico { namespace ast { namespace statements {

    class let: public statement {

    public:
        using uptr = std::unique_ptr<let>;

        using expr_type        = expression::uptr;
        using ident_type       = expression::uptr;

        let( expression::uptr id, expression::uptr val )
            :ident_(std::move(id))
            ,value_(std::move(val))
        { }

        type get_type( ) const
        {
            return type::LET;
        }

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

    private:

        expression::uptr ident_;
        expression::uptr value_;
    };

    class ret: public statement {

    public:
        using uptr = std::unique_ptr<ret>;

        ret( expression::uptr val )
            :value_(std::move(val))
        { }

        type get_type( ) const
        {
            return type::RETURN;
        }

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

    private:
        expression::uptr value_;
    };

    class expr: public statement {
    public:
        using uptr = std::unique_ptr<expr>;

        expr( expression::uptr val )
            :expr_(std::move(val))
        { }

        type get_type( ) const
        {
            return type::EXPR;
        }

        std::string str( ) const
        {
            return expr_->str( );
        }

        expression::uptr &value( )
        {
            return expr_;
        }

    private:
        expression::uptr expr_;
    };

}}}

#endif // STATEMENTS_H
