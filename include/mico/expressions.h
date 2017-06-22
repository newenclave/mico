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

        type get_type( ) const
        {
            return type::IDENT;
        }

        std::string str( ) const
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

        type get_type( ) const
        {
            return type::STRING;
        }

        std::string str( ) const
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

        type get_type( ) const
        {
            return type::INTEGER;
        }

        std::string str( ) const
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

        type get_type( ) const
        {
            return type::FLOAT;
        }

        std::string str( ) const
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

    class prefix: public expression {
    public:

        using uptr = std::unique_ptr<prefix>;

        prefix( tokens::type tt, expression::uptr exp )
            :token_(tt)
            ,expr_(std::move(exp))
        { }

        type get_type( ) const
        {
            return type::PREFIX;
        }

        std::string str( ) const
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

        type get_type( ) const
        {
            return ast::type::INFIX;
        }

        std::string str( ) const
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
