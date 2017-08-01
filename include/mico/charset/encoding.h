#ifndef MICO_CHARSET_ENCODING_H
#define MICO_CHARSET_ENCODING_H

#ifdef _WIN32
#include <windows.h>
#endif

#include "etool/charset/utf8.h"

namespace mico { namespace encoding {

#ifdef _WIN32
    namespace sys {

        using con_string      = std::string;
        using fil_string      = std::string;
        using internal_string = std::wstring;

        static
        std::string make_mb_string( LPCWSTR src, UINT CodePage = CP_ACP )
        {

            int cch = WideCharToMultiByte( CodePage, 0, src, -1, 0, 0, 0, 0 );

            if( 0 != cch ) {

                std::vector<char> data;
                data.resize( cch + 1 );

                cch = WideCharToMultiByte( CodePage, 0, src, -1, &data[0],
                            static_cast<DWORD>(data.size( )), 0, 0 );
                return &data.front();
            }
            return "";
        }

        static
        std::string make_ws_string( LPCSTR src, UINT CodePage = CP_ACP ) {

            int cch = MultiByteToWideChar( CodePage, 0, src, -1, 0, 0 );
            if( 0 != cch ) {

                std::vector<wchar_t> data;
                data.resize( cch + 1 );
                cch = MultiByteToWideChar( CodePage, 0, src, -1, &data[0],
                        static_cast<DWORD>(data.size( ) ) );

                return &data.front();
            }
            return L"";
        }

        static
        con_string to_console( const internal_string &inter )
        {
            return make_mb_string(  );
        }

        static
        internal_string from_console( const con_string &inter )
        {
            return etool::charset::utf8::utf8_to_ucs4( inter );
        }

        static
        con_string to_file( const internal_string &inter )
        {
            return etool::charset::utf8::ucs4_to_utf8( inter );
        }

        static
        internal_string from_file( const con_string &inter )
        {
            return etool::charset::utf8::utf8_to_ucs4( inter );
        }

    }
#else
    namespace sys {
        using con_string      = std::string;
        using fil_string      = std::string;
        using internal_string = etool::charset::utf8::ucs4string;
        struct charset {

            static
            con_string to_console( const internal_string &inter )
            {
                return etool::charset::utf8::ucs4_to_utf8( inter );
            }

            static
            internal_string from_console( const con_string &inter )
            {
                return etool::charset::utf8::utf8_to_ucs4( inter );
            }

            static
            con_string to_file( const internal_string &inter )
            {
                return etool::charset::utf8::ucs4_to_utf8( inter );
            }

            static
            internal_string from_file( const con_string &inter )
            {
                return etool::charset::utf8::utf8_to_ucs4( inter );
            }

        };
    }
#endif


}}

#endif // ENCODING_H
