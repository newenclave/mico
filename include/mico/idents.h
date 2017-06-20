#ifndef MICO_IDENTS_H
#define MICO_IDENTS_H

#include "mico/numeric.h"

namespace mico {
    struct idents {

        static
        bool is_whitespace( char c )
        {
            return (c == ' ')
                || (c == '\t')
                || (c == '\r')
                 ;
        }

        static
        bool is_newline( char c )
        {
            return (c == '\n')
                 ;
        }

        static
        bool is_digit( char c )
        {
            return numeric::valid_for_dec( c );
        }

        static
        bool is_alfa( char c )
        {
            return ( ('a' <= c) && (c <= 'z') )
                || ( ('A' <= c) && (c <= 'Z') )
                || ( c == '_' )
                 ;
        }

        static
        bool is_ident( char c )
        {
            return is_digit(c) || is_alfa(c) || (c == '_');
        }

    };
}

#endif // IDENTS_H
