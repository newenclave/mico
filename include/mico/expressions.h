#ifndef MICO_EXPRESSIONS_H
#define MICO_EXPRESSIONS_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"

namespace mico { namespace ast { namespace expressions {


//virtual type get_type( ) const = 0;
//virtual std::string str( ) const = 0;

    class ident: public expression {

    public:

        ident(std::string val)
            :value_(std::move(val))
        { }

        type get_type( ) const override
        {
            return type::IDENT;
        }

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

    class string: public expression {

    public:

        string(std::string val)
            :value_(std::move(val))
        { }

        type get_type( ) const override
        {
            return type::STRING;
        }

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

    class integer: public expression {
    public:

        using uptr = std::unique_ptr<integer>;

        integer( std::uint64_t val )
            :value_(val)
        { }

        type get_type( ) const override
        {
            return type::INTEGER;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << value_;
            return oss.str( );
        }

        std::uint64_t value( ) const
        {
            return value_;
        }

    private:
        std::uint64_t value_;
    };

    //// TODO: use template
    class floating: public expression {
    public:

        using uptr = std::unique_ptr<floating>;

        floating( double val )
            :value_(val)
        { }

        type get_type( ) const override
        {
            return type::FLOAT;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << value_;
            return oss.str( );
        }

        double value( ) const
        {
            return value_;
        }

    private:
        double value_;
    };

    //// TODO: use template
    class boolean: public expression {
    public:

        using uptr = std::unique_ptr<boolean>;

        boolean( bool val )
            :value_(val)
        { }

        type get_type( ) const override
        {
            return type::BOOLEAN;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << (value_ ? "true" : "false");
            return oss.str( );
        }

        bool value( ) const
        {
            return value_;
        }

    private:

        bool value_;
    };

    class prefix: public expression {
    public:

        using uptr = std::unique_ptr<prefix>;

        prefix( tokens::type tt, expression::uptr exp )
            :token_(tt)
            ,expr_(std::move(exp))
        { }

        type get_type( ) const override
        {
            return type::PREFIX;
        }

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

    class infix: public expression {
    public:

        using uptr = std::unique_ptr<infix>;

        infix( tokens::type tt, expression::uptr lft )
            :token_(tt)
            ,left_(std::move(lft))
        { }

        type get_type( ) const override
        {
            return ast::type::INFIX;
        }

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

    class function: public expression {

    public:
        using uptr = std::unique_ptr<function>;

        using stmt_type  = statement::uptr;
        using ident_type = expression::uptr;

        function( )
            :ident_(std::make_shared<expression_list>( ))
            ,expr_(std::make_shared<statement_list>( ))
        { }

        ~function( )
        { }

        type get_type( ) const override
        {
            return type::FN;
        }

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

    class call: public expression {

    public:

        using uptr = std::unique_ptr<call>;
        using param_type = expression::uptr;

        call( expression::uptr f )
            :func_(std::move(f))
        { }

        ~call(  )
        { }

        type get_type( ) const override
        {
            return type::CALL;
        }

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

    class if_expr: public expression {

    public:

        using uptr = std::unique_ptr<if_expr>;

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

        type get_type( ) const override
        {
            return type::IFELSE;
        }

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

    class table: public expression {
    public:

        using value_pair = std::pair<expression::uptr, expression::uptr>;
        using value_type = std::vector<value_pair>;
        using uptr = std::unique_ptr<table>;

        type get_type( ) const override
        {
            return type::TABLE;
        }

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

    class null: public expression {

    public:

        type get_type( ) const
        {
            return type::NONE;
        }

        std::string str( ) const
        {
            return "null";
        }
    };

}}}

#endif // EXPRESSIONS_H
