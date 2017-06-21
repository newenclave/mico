#ifndef MICO_NUMERIC_H
#define MICO_NUMERIC_H

#include "etool/slices/container.h"
#include "mico/tokens.h"

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
        bool valid_for_dec_int( int c )
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

        template <typename ItrT>
        static
        bool check_if_float( ItrT begin, ItrT end )
        {
            bool float_found = false;
            while( begin != end && !float_found ) {
                char c = *begin++;
                if( (c == 'e') || (c == 'E') || (c == '.') ) {
                    float_found = true;
                } else if( !valid_for_dec( c ) ) {
                    return false;
                }
            }

            return float_found;
        }

        template <typename ContT>
        static
        double parse_float( const ContT &cont )
        {
            auto bb = std::begin(cont);
            return parse_float( bb, std::end(cont) );
        }

        template <typename ItrT>
        static
        double parse_float( ItrT &s, ItrT end )
        {
            double a = 0.0;
            int    e = 0;
            int    c = 0;

            while( (s != end) && valid_for_dec_int(c = *s++) ) {
                a = a * 10.0 + (c - '0');
            }

            if( c == '.' ) {
                while( (s != end) && valid_for_dec_int(c = *s++)) {
                    a = a*10.0 + (c - '0');
                    e = e-1;
                }
            }

            if( (c == 'e' || c == 'E') ) {
                int sign = 1;
                int i = 0;
                c = *s++;

                if (c == '+') {
                    c = *s++;
                } else if (c == '-') {
                    c = *s++;
                    sign = -1;
                }

                while( valid_for_dec_int( c ) ) {
                    i = i * 10 + (c - '0');
                    if( s == end ) {
                        break;
                    } else {
                        c = *s++;
                    }
                }
                e += i*sign;
            }

            while( e > 0 ) {
                a *= 10.0;
                e--;
            }

            while( e < 0 ) {
                a *= 0.1;
                e++;
            }

            if( s != end ) {
                --s;
            }
            return a;
        }

        static
        bool valid_for( tokens::type t, char c )
        {
            switch( t ) {
            case tokens::type::INT_BIN:
                return valid_for_bin( c );
            case tokens::type::INT_TER:
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
