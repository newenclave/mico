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

        let( )
        { }

        type get_type( ) const
        {
            return type::LET;
        }

        std::string str( ) const
        {
            std::ostringstream oss;
            oss << "let ";
            bool second = false;
            for( auto &id: idents( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << id->str( );
            }
            oss << " = ";
            second = false;
            for( auto &ex: exprs( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << ex->str( );
            }
            return oss.str( );
        }

        const expression_list &idents( ) const
        {
            return ident_;
        }

        expression_list &idents( )
        {
            return ident_;
        }

        const expression_list &exprs( ) const
        {
            return expr_;
        }

        expression_list &exprs( )
        {
            return expr_;
        }

        void push_ident( ident_type val )
        {
            ident_.emplace_back(std::move(val));
        }

        void push_exp( expr_type val )
        {
            expr_.emplace_back(std::move(val));
        }

    private:

        expression_list ident_;
        expression_list expr_;
    };

    class ret: public statement {

    public:
        using uptr = std::unique_ptr<ret>;

        ret(  )
        { }

        type get_type( ) const
        {
            return type::RETURN;
        }

        std::string str( ) const
        {
            std::ostringstream oss;
            oss << "return ";
            bool second = false;
            for( auto &ex: exprs( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << ex->str( );
            }
            return oss.str( );
        }

        const expression_list &exprs( ) const
        {
            return expr_;
        }

        expression_list &exprs( )
        {
            return expr_;
        }

        void push_exp( expression::uptr val )
        {
            expr_.emplace_back(std::move(val));
        }

    private:
        expression_list expr_;
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
