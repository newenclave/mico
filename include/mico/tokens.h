#ifndef MICO_TOKENS_H
#define MICO_TOKENS_H

namespace mico { namespace tokens {

    enum class type {

        NONE = 0,
        END_OF_FILE,
        END_OF_LINE,
        IDENT,
        INT,
        FLOAT,

        FIRST_VISIBLE = 100,

        /// keywords
        LET,
        RETURN,
        FUNCTION,

        /// operators

        LAST_VISIBLE,
    };

    struct name {

        static
        const char *get( type t )
        {
            switch( t ) {
            case type::NONE:
                return "";

            case type::END_OF_FILE:
                return "EOF";
            case type::END_OF_LINE:
                return "EOL";
            case type::IDENT:
                return "IDENT";
            case type::INT:
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

            ////////// unnamed
            case type::FIRST_VISIBLE:
            case type::LAST_VISIBLE:
                break;
            }
            return "";
        }
    };

}}

#endif // TOKENS_H
