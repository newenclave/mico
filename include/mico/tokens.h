#ifndef MICO_TOKENS_H
#define MICO_TOKENS_H

#include <cstdint>
#include <utility>
#include <string>

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
    struct type_value {
        using value_type = Cont;

        type_value( ) = default;
        type_value( type_value & ) = default;
        type_value& operator = ( type_value & ) = default;

        type_value( type_value &&other )
            :t(other.t)
            ,value(std::move(other.value))
        { }

        type_value& operator = ( type_value &&other )
        {
            t     = other.t;
            value = std::move(other.value);
            return *this;
        }

        type        t;
        value_type  value;
    };

    template <typename Cont = std::string>
    struct info {

        using value_type = Cont;

        info( ) = default;
        info( info & ) = default;
        info& operator = ( info & ) = default;

        info( info &&other )
            :line(other.line)
            ,pos(other.pos)
            ,tv(std::move(other.tv))
        { }

        info& operator = ( info &&other )
        {
            line = other.line;
            pos  = other.pos;
            tv   = std::move(other.tv);
            return *this;
        }

        std::size_t line = 0;
        std::size_t pos  = 0;
        type_value<value_type> tv;
    };

}}

#endif // TOKENS_H
