#ifndef MICO_EVAL_TABLES_H
#define MICO_EVAL_TABLES_H

#include "mico/tokens.h"
#include "mico/eval/operations/common_operations.h"

namespace mico { namespace eval {
    template <>
    struct operation<objects::type::TABLE> {

        using table_type = objects::derived<objects::type::STRING>;
        using str_type   = objects::derived<objects::type::STRING>;
        using bool_type  = objects::derived<objects::type::BOOLEAN>;
        using error_type = objects::derived<objects::type::FAILURE>;
        using int_type   = objects::derived<objects::type::INTEGER>;
        using prefix     = ast::expressions::prefix;
        using infix      = ast::expressions::infix;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            auto tt = objects::cast_table(obj);
            if( pref->token( ) == tokens::type::ASTERISK ) {
                return int_type::make( tt->value( ).size( ) );
            }
            return error_type::make( pref->pos( ),   "Prefix operator '",
                                     pref->token( ), "' is not defined for"
                                                     " string");
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            using CO = common_operations;
            objects::sptr right = ev( inf->right( ).get( ) );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }

            switch (right->get_type( )) {
            case objects::type::BUILTIN:
                if(inf->token( ) == tokens::type::BIT_OR) {
                    return CO::eval_builtin( inf, obj, right, env);
                }
                break;
            case objects::type::FUNCTION:
                if(inf->token( ) == tokens::type::BIT_OR) {
                    return CO::eval_func( inf, obj, right, env);
                }
                break;
            }

            return error_type::make(inf->pos( ), "Infix operation ",
                                    obj->get_type( )," '",
                                    inf->token( ), "' ",
                                    right->get_type( ),
                                    " is not defined");
        }

    };
}}

#endif // TABLES_H
