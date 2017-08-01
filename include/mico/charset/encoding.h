#ifndef MICO_CHARSET_ENCODING_H
#define MICO_CHARSET_ENCODING_H

#include <ostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "etool/charset/utf8.h"

namespace mico { namespace charset {

    using file_string     = std::string;

#ifdef _WIN32
    using con_string      = std::string;
    using sys_string      = std::wstring;
    using internal_string = std::wstring;

    struct encoding {

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
        std::wstring make_ws_string( LPCSTR src, UINT CodePage = CP_ACP ) {

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
        con_string to_console( const internal_string &enter )
        {
            return make_mb_string( enter.c_str( ), CP_OEMCP );
        }

        static
        internal_string from_console( const con_string &enter )
        {
            return make_ws_string( enter.c_str( ), CP_OEMCP );
        }

        static
        file_string to_file( const internal_string &enter )
        {
            return make_mb_string( enter.c_str( ), CP_UTF8 );
        }

        static
        internal_string from_file( const file_string &enter )
        {
            return make_ws_string( enter.c_str( ), CP_UTF8 );
        }

        static
        sys_string to_file( const internal_string &enter )
        {
            return enter;
        }

        static
        internal_string from_sys( const sys_string &enter )
        {
            return enter;
        }
    };
#else

    using con_string      = std::string;
    using sys_string      = std::string;
    using internal_string = etool::charset::utf8::ucs4string;

    struct encoding {

        static
        con_string to_console( const internal_string &enter )
        {
            return etool::charset::utf8::ucs4_to_utf8( enter );
        }

        static
        internal_string from_console( const con_string &enter )
        {
            return etool::charset::utf8::utf8_to_ucs4( enter );
        }

        static
        file_string to_file( const internal_string &enter )
        {
            return etool::charset::utf8::ucs4_to_utf8( enter );
        }

        static
        internal_string from_file( const file_string &enter )
        {
            return etool::charset::utf8::utf8_to_ucs4( enter );
        }

        static
        sys_string to_sys( const internal_string &enter )
        {
            return etool::charset::utf8::ucs4_to_utf8( enter );
        }

        static
        internal_string from_sys( const sys_string &enter )
        {
            return etool::charset::utf8::utf8_to_ucs4( enter );
        }
    };
#endif

}}

#endif // ENCODING_H
