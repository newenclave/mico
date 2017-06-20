#ifndef MICO_NUMERIC_H
#define MICO_NUMERIC_H

#include "etool/slices/container.h"
#include "tokens.h"

namespace mico {

    struct numeric {

        static
        bool valid_for_bin( char c )
        {
            return ('0' == c) || (c == '1');
        }

        static
        bool valid_for_tre( char c )
        {
            return ('0' <= c) && (c <= '2');
        }

        static
        bool valid_for_oct( char c )
        {
            return ('0' <= c) && (c <= '7' );
        }

        static
        bool valid_for_dec( char c )
        {
            return ('0' <= c) && (c <= '9' );
        }

        static
        bool valid_for_hex( char c )
        {
            return (('0' <= c) && (c <= '9' ))
                || (('a' <= c) && (c <= 'f' ))
                || (('A' <= c) && (c <= 'F' ))
                 ;
        }

        static
        bool isdigit( char c )
        {
            return valid_for_dec( c );
        }

        static
        bool valid_for( tokens::type t, char c )
        {
            switch( t ) {
            case tokens::type::INT_BIN:
                return valid_for_bin( c );
            case tokens::type::INT_TRE:
                return valid_for_tre( c );
            case tokens::type::INT_OCT:
                return valid_for_oct( c );
            case tokens::type::INT_DEC:
                return valid_for_dec( c );
            case tokens::type::INT_HEX:
                return valid_for_hex( c );
            default:
                break;
            }
            return false;
        }
    };

}

#endif // NUMERIC_H
