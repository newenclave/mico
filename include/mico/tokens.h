#ifndef MICO_TOKENS_H
#define MICO_TOKENS_H

#include <cstdint>
#include <utility>
#include <string>
#include <ostream>

namespace mico { namespace tokens {

    enum class type {

        NONE        = 0,
        END_OF_LINE,
        END_OF_FILE,

        INT_BIN,
        INT_TRE,
        INT_OCT,
        INT_DEC,
        INT_HEX,

        FLOAT,

        IDENT,

        FIRST_VISIBLE = 100,

        /// symbols

        SEMICOLON,
        COLON,
        DOT,
        COMMA,
        BANG,
        ASTERISK,
        SLASH,
        EQ,
        NOT_EQ,

        /// keywords
        LET,
        RETURN,
        FUNCTION,

        /// operators
        MINUS,
        PLUS,

        LAST_VISIBLE,
    };

    struct name {

        static
        const char *get( type t )
        {
            switch( t ) {
            case type::END_OF_FILE:
                return "EOF";
            case type::END_OF_LINE:
                return "EOL";
            case type::IDENT:
                return "IDENT";

            case type::INT_BIN:
            case type::INT_TRE:
            case type::INT_OCT:
            case type::INT_DEC:
            case type::INT_HEX:
                return "INTEGER";
            case type::FLOAT:
                return "FLOAT";

            ////////// tokens that have names

            case type::SEMICOLON:
                 return ";";
            case type::COLON:
                 return ":";
            case type::DOT:
                 return ".";
            case type::COMMA:
                 return ",";

            case type::BANG:
                return "!";
            case type::ASTERISK:
                return "*";
            case type::SLASH:
                return "/";
            case type::EQ:
                 return "=";
            case type::NOT_EQ:
                 return "!=";

            case type::LET:
                return "let";
            case type::RETURN:
                return "return";
            case type::FUNCTION:
                return "fn";

            /// operators
            case type::MINUS:
                return "-";
            case type::PLUS:
                return "+";

            ////////// unnamed
            case type::NONE:
            case type::FIRST_VISIBLE:
            case type::LAST_VISIBLE:
                break;
            }
            return "";
        }
    };

    template <typename Cont = std::string>
    struct type_ident {

        using value_type = Cont;

        type_ident( ) = default;
        type_ident( const type_ident & ) = default;
        type_ident& operator = ( const type_ident & ) = default;

        type_ident( type tt )
            :name(tt)
            ,literal(name::get(tt))
        { }

        type_ident( type tt, std::string val )
            :name(tt)
            ,literal(std::move(val))
        { }

        type_ident( type_ident &&other )
            :name(other.name)
            ,literal(std::move(other.literal))
        { }

        type_ident& operator = ( type_ident &&other )
        {
            name     = other.name;
            literal = std::move(other.literal);
            return *this;
        }

        type        name;
        value_type  literal;
    };

    template <typename Cont = std::string>
    struct info {

        using value_type = Cont;

        info( ) = default;
        info( const info & ) = default;
        info& operator = ( const info & ) = default;


        info( type t )
            :ident(t)
        { }

        info( type t, std::string value )
            :ident(t, std::move(value))
        { }

        info( info &&other )
            :line(other.line)
            ,pos(other.pos)
            ,ident(std::move(other.ident))
        { }

        info& operator = ( info &&other )
        {
            line = other.line;
            pos  = other.pos;
            ident   = std::move(other.ident);
            return *this;
        }

        std::size_t line = 0;
        std::size_t pos  = 0;
        type_ident<value_type> ident;
    };

    inline
    std::ostream &operator << (std::ostream &o, tokens::type tt )
    {
        return o << name::get(tt);
    }

}}

#endif // TOKENS_H
