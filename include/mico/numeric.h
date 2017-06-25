#ifndef MICO_NUMERIC_H
#define MICO_NUMERIC_H

#include "etool/slices/container.h"
#include "mico/tokens.h"

namespace mico {

    struct numeric {

        static
        std::uint8_t char2int( char cc )
        {
            std::uint8_t c = static_cast<std::uint8_t>(cc);
            switch (c) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return c - '0';
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                return c - 'a' + 0xa;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                return c - 'A' + 0xA;
            }
            return 0xFF;
        }

        template <typename CharT>
        static
        bool valid_for_bin( CharT c )
        {
            return ('0' == c) || (c == '1');
        }

        template <typename CharT>
        static
        bool valid_for_ter( CharT c )
        {
            return ('0' <= c) && (c <= '2');
        }

        template <typename CharT>
        static
        bool valid_for_oct( CharT c )
        {
            return ('0' <= c) && (c <= '7' );
        }

        template <typename CharT>
        static
        bool valid_for_dec( CharT c )
        {
            return ('0' <= c) && (c <= '9' );
        }

        template <typename CharT>
        static
        bool valid_for_hex( CharT c )
        {
            return (('0' <= c) && (c <= '9' ))
                || (('a' <= c) && (c <= 'f' ))
                || (('A' <= c) && (c <= 'F' ))
                 ;
        }

        static
        bool valid_for( tokens::type t, char c )
        {
            switch( t ) {
            case tokens::type::INT_BIN:
                return valid_for_bin( c );
            case tokens::type::INT_TER:
                return valid_for_ter( c );
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

        static
        unsigned base_for( tokens::type t )
        {
            switch( t ) {
            case tokens::type::INT_BIN:
                return 2;
            case tokens::type::INT_TER:
                return 3;
            case tokens::type::INT_OCT:
                return 8;
            case tokens::type::INT_DEC:
                return 10;
            case tokens::type::INT_HEX:
                return 16;
            default:
                break;
            }
            return 0;
        }

        static
        std::uint64_t parse_int( const std::string &input, tokens::type tt,
                                 int *first_inval )
        {
            std::uint64_t res = 0;
            *first_inval = -1;

            int pos = 0;

            for( auto c: input ) {
                if( valid_for(tt, c) ) {
                    res *= base_for( tt );
                    res += char2int( c );
                    ++pos;
                } else {
                    *first_inval = pos;
                    return 0;
                }
            }

            return res;
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

            while( (s != end) && valid_for_dec(c = *s++) ) {
                a = a * 10.0 + (c - '0');
            }

            if( c == '.' ) {
                while( (s != end) && valid_for_dec(c = *s++)) {
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

                while( valid_for_dec( c ) ) {
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

    };

}

#endif // NUMERIC_H
