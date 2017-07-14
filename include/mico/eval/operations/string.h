#ifndef MICO_EVL_STR_OPERATION_H
#define MICO_EVL_STR_OPERATION_H

#include "mico/tokens.h"
#include "mico/objects/error.h"
#include "mico/objects/string.h"
#include "mico/objects/numbers.h"
#include "mico/expressions/expressions.h"

namespace mico { namespace eval {

    template <>
    struct operation<objects::type::STRING> {

        using str_type   = objects::derived<objects::type::STRING>;
        using bool_type  = objects::derived<objects::type::BOOLEAN>;
        using error_type = objects::derived<objects::type::FAILURE>;
        using int_type   = objects::derived<objects::type::INTEGER>;
        using prefix     = ast::expressions::prefix;
        using infix      = ast::expressions::infix;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "string");
        }

        static
        objects::sptr eval_str( infix *inf, const std::string &lft,
                                 const std::string &rht )
        {
            switch( inf->token( )) {
            case tokens::type::PLUS:
                return  str_type::make( lft  + rht );
            case tokens::type::EQ:
                return bool_type::make( lft == rht );
            case tokens::type::NOT_EQ:
                return bool_type::make( lft != rht );
            case tokens::type::LT:
                return bool_type::make( lft  < rht );
            case tokens::type::LT_EQ:
                return bool_type::make( lft <= rht );
            case tokens::type::GT:
                return bool_type::make( lft  > rht );
            case tokens::type::GT_EQ:
                return bool_type::make( lft >= rht );
            }

            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "strings");

        }
        static
        objects::sptr eval_int( infix *inf, const std::string &lft,
                                 std::int64_t rht )
        {
            switch( inf->token( )) {
            case tokens::type::ASTERISK: {
                if( rht > 0 ) {
                    std::string res;
                    res.reserve( lft.size( ) * rht );
                    while( rht-- ) {
                        res.insert( res.end( ), lft.begin( ), lft.end( ) );
                    }
                    return str_type::make( std::move(res) );
                } else if( rht == 0  ) {
                    return str_type::make( "" );
                } else {
                    return str_type::make( lft );
                }
            }
            }
            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "string and integer");

        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr /* env */ )
        {
            auto val = static_cast<str_type *>(obj.get( ));

            objects::sptr right = ev( inf->right( ).get( ) );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }
            switch (right->get_type( )) {
            case objects::type::STRING: {
                auto rght = static_cast<str_type *>(right.get( ));
                return eval_str( inf, val->value( ), rght->value( ) );
            }
            case objects::type::INTEGER: {
                auto rght = static_cast<int_type *>(right.get( ));
                return eval_int( inf, val->value( ), rght->value( ) );
            }
            }
            return error_type::make(inf->pos( ), "Infix operation ",
                                    obj->get_type( )," '",
                                    inf->token( ), "' ",
                                    right->get_type( ),
                                    " is not defined");
        }
    };
}}

#endif // STR_OPERATION_H
