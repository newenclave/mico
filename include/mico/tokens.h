#ifndef MICO_TOKENS_H
#define MICO_TOKENS_H

#include <cstdint>
#include <utility>
#include <string>
#include <ostream>

#include "etool/slices/container.h"

namespace mico { namespace tokens {

    enum class type {

        NONE        = 0,
        INVALID,
        END_OF_LINE,
        END_OF_FILE,
        COMMENT,

        INT_BIN,
        INT_TER,
        INT_OCT,
        INT_DEC,
        INT_HEX,
        FLOAT,
        IDENT,
        STRING,

        FIRST_VISIBLE = 100,

        /// symbols

        SEMICOLON,
        COLON,
        DOT,
        COMMA,

        /// operators
        ASSIGN,
        MINUS,
        PLUS,
        BANG,
        ASTERISK,
        SLASH,
        EQ,
        NOT_EQ,
        LT,
        GT,
        LT_EQ,
        GT_EQ,

        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,

        /// keywords
        LET,
        RETURN,
        FUNCTION,
        BOOL_TRUE,
        BOOL_FALSE,
        IF,
        ELIF,
        ELSE,

        LAST_VISIBLE,
    };

    struct name {

        static
        const char *get( type t )
        {
            switch( t ) {
            case type::INVALID:     return "INVALID";
            case type::END_OF_FILE: return "EOF";
            case type::END_OF_LINE: return "EOL";
            case type::COMMENT:     return "COMMENT";

            case type::IDENT:       return "IDENT";

            case type::INT_BIN:     return "INT_2";
            case type::INT_TER:     return "INT_3";
            case type::INT_OCT:     return "INT_8";
            case type::INT_DEC:     return "INT_10";
            case type::INT_HEX:     return "INT_16";
            case type::FLOAT:       return "FLOAT";
            case type::STRING:      return "STRING";

            /// tokens that have names
            case type::SEMICOLON:   return ";";
            case type::COLON:       return ":";
            case type::DOT:         return ".";
            case type::COMMA:       return ",";

            /// operators
            case type::ASSIGN:      return "=";
            case type::MINUS:       return "-";
            case type::PLUS:        return "+";
            case type::BANG:        return "!";
            case type::ASTERISK:    return "*";
            case type::SLASH:       return "/";
            case type::EQ:          return "==";
            case type::NOT_EQ:      return "!=";
            case type::LT:          return "<";
            case type::GT:          return ">";
            case type::LT_EQ:       return "<=";
            case type::GT_EQ:       return ">=";

            case type::LPAREN:      return "(";
            case type::RPAREN:      return ")";
            case type::LBRACE:      return "{";
            case type::RBRACE:      return "}";
            case type::LBRACKET:    return "[";
            case type::RBRACKET:    return "]";

            /// keywords
            case type::LET:         return "let";
            case type::RETURN:      return "return";
            case type::FUNCTION:    return "fn";
            case type::BOOL_TRUE:   return "true";
            case type::BOOL_FALSE:  return "false";
            case type::IF:          return "if";
            case type::ELIF:        return "elif";
            case type::ELSE:        return "else";

            /// unnamed
            case type::NONE:
            case type::FIRST_VISIBLE:
            case type::LAST_VISIBLE:
                break;
            }
            return "";
        }
    };

    struct type_ident {

        using value_type = std::string;
        using slice = etool::slices::container<value_type::const_iterator>;

        type_ident( ) = default;
        type_ident( const type_ident & ) = default;
        type_ident& operator = ( const type_ident & ) = default;

        type_ident( tokens::type tt )
            :name_value_(tt)
        { }

        type_ident( tokens::type tt, slice val )
            :lit_value_(std::move(val))
            ,name_value_(tt)
        { }

        type_ident( type_ident &&other )
            :lit_value_(std::move(other.lit_value_))
            ,name_value_(other.name( ))
        { }

        type_ident& operator = ( type_ident &&other )
        {
            name_value_ = other.name( );
            lit_value_  = std::move(other.lit_value_);
            return *this;
        }

        std::string literal( ) const
        {
            return lit_value_.empty( )
                 ? tokens::name::get( name( ) )
                 : std::string(lit_value_.begin( ), lit_value_.end( ));
        }

        slice::iterator begin( ) const
        {
            return lit_value_.begin( );
        }

        slice::iterator end( ) const
        {
            return lit_value_.end( );
        }

        tokens::type name( ) const
        {
            return name_value_;
        }

        void set_name( tokens::type val )
        {
            name_value_ = val;
        }

        void set_literal( slice val )
        {
            lit_value_ = std::move(val);
        }

    private:
        slice        lit_value_;
        tokens::type name_value_;
    };

    struct position {
        std::size_t line = 0;
        std::size_t pos  = 0;
        position(std::size_t l, std::size_t p )
            :line(l)
            ,pos(p)
        { }
        position(  ) = default;
    };

    struct info {

        using value_type = std::string;
        using slice = etool::slices::container<value_type::const_iterator>;

        info( ) = default;
        info( const info & ) = default;
        info& operator = ( const info & ) = default;


        info( tokens::type t )
            :ident(t)
        { }

        info( tokens::type t, slice value )
            :ident(t, std::move(value))
        { }

        info( info &&other )
            :where(other.where)
            ,ident(std::move(other.ident))
        { }

        info& operator = ( info &&other )
        {
            where = other.where;
            ident = std::move(other.ident);
            return *this;
        }

        tokens::position where;
        type_ident       ident;
    };

    inline
    std::ostream &operator << (std::ostream &o, tokens::type tt )
    {
        return o << name::get(tt);
    }

    inline
    std::ostream &operator << (std::ostream &o, const tokens::position &tt )
    {
        return o << tt.line << ":" << tt.pos;
    }

    inline
    std::ostream &operator << (std::ostream &o, const tokens::type_ident &ti )
    {
        o << ti.name( );
        if( ti.name( ) < tokens::type::FIRST_VISIBLE ||
                         tokens::type::LAST_VISIBLE < ti.name( ) )
        {
            o << "(" << ti.literal( ) << ")";
        }
        return o;
    }

}}

#endif // TOKENS_H
