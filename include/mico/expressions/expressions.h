#ifndef MICO_EXPRESSIONS_H
#define MICO_EXPRESSIONS_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"
#include "mico/expressions/fn.h"
#include "mico/expressions/value.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::IDENT>: public typed_expr<type::IDENT> {

        using this_type = detail<type::IDENT>;

    public:

        using uptr = std::unique_ptr<this_type>;

        detail(std::string val)
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            return value_;
        }

        const std::string &value( ) const
        {
            return value_;
        }

    private:
        std::string value_;
    };

    template <>
    class detail<type::STRING>: public typed_expr<type::STRING> {

        using this_type = detail<type::STRING>;
    public:
        using uptr = std::unique_ptr<this_type>;

        detail(std::string val)
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            return std::string("\"") + value_ + '"';
        }

        const std::string &value( ) const
        {
            return value_;
        }

        std::string &value( )
        {
            return value_;
        }

    private:
        std::string value_;
    };

    template <>
    class detail<type::ARRAY>: public typed_expr<type::ARRAY> {
        using this_type = detail<type::ARRAY>;
    public:

        using uptr = std::unique_ptr<this_type>;
        using value_type = std::vector<expression::uptr>;

        std::string str( ) const override
        {
            std::stringstream oss;

            bool first = true;

            oss << "[";
            for( auto &e: value( ) ) {
                if( !first ) {
                    oss << ", ";
                } else {
                    first = true;
                }
                oss << e->str( );
            }
            oss << "]";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

    private:

        value_type value_;
    };

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

    private:
        tokens::type     token_;
        expression::uptr expr_;
    };

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

    template <>
    class detail<type::CALL>: public typed_expr<type::CALL> {

    public:

        using uptr = std::unique_ptr<detail>;
        using param_type = expression::uptr;

        detail( expression::uptr f )
            :expr_(std::move(f))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << expr_->str( ) << "(";
            bool second = false;
            for( auto &par: params( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << par->str( );
            }
            oss << ")";
            return oss.str( );
        }

        const expression_list &params( ) const
        {
            return params_;
        }

        expression_list &params( )
        {
            return params_;
        }

        expression *func( )
        {
            return expr_.get( );
        }

        const expression *func( ) const
        {
            return expr_.get( );
        }

    private:
        expression::uptr expr_;
        expression_list  params_;
    };

    template <>
    class detail<type::IFELSE>: public typed_expr<type::IFELSE> {

    public:

        using uptr = std::unique_ptr<detail>;

        struct node {
            node( expression::uptr val )
                :cond(std::move(val))
            { }

            node( ) = default;
            node( node && ) = default;
            node &operator = ( node && ) = default;

            node ( node & ) = delete;
            node &operator = ( node & ) = delete;

            expression::uptr cond;
            statement_list   states;
        };

        using if_list = std::vector<node>;

        std::string str( ) const override
        {
            std::ostringstream oss;

            bool first = true;
            for( auto &f: general_ ) {
                oss << (first ? "if " : " elif " )
                    << f.cond->str( ) << " {\n";
                for( auto &c: f.states ) {
                    oss << c->str( ) << ";\n";
                }
                oss << "}";
                first = false;
            }
            if( !alt_.empty( ) ) {
                oss << " else {\n";
                for( auto &a: alt_ ) {
                    oss << a->str( ) << ";\n";
                }
                oss << "}";
            }

            return oss.str( );
        }

        if_list &ifs( )
        {
            return general_;
        }

        statement_list &alt( )
        {
            return alt_;
        }

    private:
        if_list         general_;
        statement_list  alt_;
    };

    template <>
    class detail<type::TABLE>: public typed_expr<type::TABLE> {
    public:

        using value_pair = std::pair<expression::uptr, expression::uptr>;
        using value_type = std::vector<value_pair>;
        using uptr = std::unique_ptr<detail>;

        std::string str( ) const override
        {
            std::ostringstream oss;

            bool first = true;
            for( auto &v: value_ ) {
                if( first ) {
                    first = false;
                } else {
                    oss << ", ";
                }
                oss << v.first->str( ) << ":" << v.second->str( );
            }

            return oss.str( );
        }

        value_type &value( )
        {
            return value_;
        }

        const value_type &value( ) const
        {
            return value_;
        }

    private:

        value_type value_;
    };

    template <>
    class detail<type::NONE>: public typed_expr<type::NONE> {

    public:
        std::string str( ) const
        {
            return "null";
        }
    };

    using ident     = detail<type::IDENT>;
    using string    = detail<type::STRING>;
    using array     = detail<type::ARRAY>;
    using integer   = detail<type::INTEGER>;
    using floating  = detail<type::FLOAT>;
    using boolean   = detail<type::BOOLEAN>;

    using prefix    = detail<type::PREFIX>;
    using infix     = detail<type::INFIX>;
    using index     = detail<type::INDEX>;

    using function  = detail<type::FN>;
    using call      = detail<type::CALL>;

    using ifelse    = detail<type::IFELSE>;
    using table     = detail<type::TABLE>;
    using null      = detail<type::NONE>;

}}}

#endif // EXPRESSIONS_H
