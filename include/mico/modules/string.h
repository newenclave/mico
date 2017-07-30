#ifndef MICO_MODULES_STRING_H
#define MICO_MODULES_STRING_H

#include "mico/builtin.h"
#include "mico/builtin/common.h"
#include "mico/objects/module.h"

#include "mico/numeric.h"

namespace mico { namespace modules {

    struct to_float {

        using ERR = objects::error;
        using INT = objects::integer;
        using FLT = objects::floating;

        objects::sptr operator ( )( objects::slist &par, environment::sptr )
        {
            if( par.size( ) > 0 ) {
                if( par[0]->get_type( ) != objects::type::STRING ) {
                    return ERR::make( par[0]->get_type( ),
                            " is not a string" );
                }
                auto str = objects::cast_string( par[0].get( ) )->value( );
                if( str.empty( ) ) {
                    return FLT::make( 0.0 );
                }
                bool minus = str[0] == '-';
                if( minus ) {
                    str.erase(str.begin( ));
                }

                auto res = numeric::parse_float( str );
                return FLT::make( minus ? -res : res );

            }
            return objects::null::make( );
        }
    };

    struct to_int {

        using ERR = objects::error;
        using INT = objects::integer;
        using FLT = objects::floating;

        static
        tokens::type get_token( const std::string &inp )
        {
            if( (inp.size( ) > 1) && (inp[0] == '0') ) {
                switch ( inp[1] ) {
                case 'b':
                case 'B':
                    return tokens::type::INT_BIN;
                case 't':
                case 'T':
                    return tokens::type::INT_TER;
                case 'x':
                case 'X':
                    return tokens::type::INT_HEX;
                default:
                    return tokens::type::INT_OCT;
                }
            }
            return tokens::type::INT_DEC;
        }

        objects::sptr operator ( )( objects::slist &par, environment::sptr )
        {
            if( par.size( ) > 0 ) {
                if( par[0]->get_type( ) != objects::type::STRING ) {
                    return ERR::make( par[0]->get_type( ),
                            " is not a string" );
                }
                auto str = objects::cast_string( par[0].get( ) )->value( );
                if( str.empty( ) ) {
                    return INT::make( 0 );
                }
                bool minus = str[0] == '-';
                if( minus ) {
                    str.erase(str.begin( ));
                }

                auto tt = get_token( str );
                if( tt == tokens::type::INT_BIN
                 || tt == tokens::type::INT_TER
                 || tt == tokens::type::INT_HEX )
                {
                    str.erase(str.begin( ), str.begin( ) + 2);
                }

                int inval = -1;
                auto res = numeric::parse_int( str, tt, &inval );

                if( inval > 0 ) {
                    auto is = static_cast<std::size_t>(inval);
                    if( is < str.size( ) ) {
                        ERR::make( par[0]->get_type( ),
                                " Invalid symbol '",  str[is],
                                "' in the string" );
                    }
                }
                return INT::make(minus ? -res : res);
            }
            return objects::null::make( );
        }
    };

    struct string {
        static
        void load( environment::sptr &env, const std::string &name = "string" )
        {
            using BIC = builtin::common;

            auto mod_e = environment::make(env);
            auto mod = objects::module::make( mod_e, name );

            mod_e->set( "to_int",   BIC::make( mod_e, to_int { } ) );
            mod_e->set( "to_float", BIC::make( mod_e, to_float { } ) );
            env->set( name, mod );
        }
    };


}}
#endif // STRING_H
