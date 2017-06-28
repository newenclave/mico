#ifndef MICO_EXPRESSIONS_H
#define MICO_EXPRESSIONS_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"

namespace mico { namespace ast { namespace expressions {

    template <type>
    class detail;

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

    public:

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

    private:
        std::string value_;
    };


    template <type TN, typename ValueT>
    class value_expr: public typed_expr<TN> {

    public:

        using uptr       = std::unique_ptr<value_expr>;
        using value_type = ValueT;

        value_expr( value_type val )
            :value_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << std::boolalpha << value_;
            return oss.str( );
        }

        value_type value( ) const
        {
            return value_;
        }

    private:
        value_type value_;
    };

    template <>
    class detail<type::INTEGER>:
            public value_expr<type::INTEGER, std::int64_t> {
    public:
        using uptr = std::unique_ptr<detail>;
        detail( std::int64_t val )
            :value_expr(val)
        { }
    };

    template <>
    class detail<type::FLOAT>:
            public value_expr<type::FLOAT, double> {
    public:
        using uptr = std::unique_ptr<detail>;
        detail( double val )
            :value_expr(val)
        { }
    };

    template <>
    class detail<type::BOOLEAN>:
            public value_expr<type::BOOLEAN, bool> {
    public:
        using uptr = std::unique_ptr<detail>;
        detail( bool val )
            :value_expr(val)
        { }
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

        expression *left( )
        {
            return left_.get( );
        }

        expression *right( )
        {
            return right_.get( );
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
    class detail<type::FN>: public typed_expr<type::FN> {

    public:
        using uptr = std::unique_ptr<detail>;

        using stmt_type  = statement::uptr;
        using ident_type = expression::uptr;

        detail( )
            :ident_(std::make_shared<expression_list>( ))
            ,expr_(std::make_shared<statement_list>( ))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(";
            bool second = false;
            for( auto &id: idents( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << id->str( );
            }
            oss << ") {\n";
            for( auto &ex: states( ) ) {
                oss << ex->str( ) << ";\n";
            }
            oss << "}";
            return oss.str( );
        }

        std::shared_ptr<expression_list> ident_ptr( )
        {
            return ident_;
        }

        std::shared_ptr<statement_list> expr_ptr( )
        {
            return expr_;
        }

        const expression_list &idents( ) const
        {
            return *ident_;
        }

        expression_list &idents( )
        {
            return *ident_;
        }

        const statement_list &states( ) const
        {
            return *expr_;
        }

        statement_list &states( )
        {
            return *expr_;
        }

    private:

        std::shared_ptr<expression_list> ident_;
        std::shared_ptr<statement_list> expr_;
    };

    template <>
    class detail<type::CALL>: public typed_expr<type::CALL> {

    public:

        using uptr = std::unique_ptr<detail>;
        using param_type = expression::uptr;

        detail( expression::uptr f )
            :func_(std::move(f))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << func_->str( ) << "(";
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
            return func_.get( );
        }

        const expression *func( ) const
        {
            return func_.get( );
        }

    private:
        expression::uptr func_;
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
    using integer   = detail<type::INTEGER>;
    using floating  = detail<type::FLOAT>;
    using boolean   = detail<type::BOOLEAN>;

    using prefix    = detail<type::PREFIX>;
    using infix     = detail<type::INFIX>;

    using function  = detail<type::FN>;
    using call      = detail<type::CALL>;

    using ifelse    = detail<type::IFELSE>;
    using table     = detail<type::TABLE>;
    using null      = detail<type::NONE>;

}}}

#endif // EXPRESSIONS_H
