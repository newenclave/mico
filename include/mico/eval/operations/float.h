#ifndef MICO_EVAL_FLOAT_OPERATION_H
#define MICO_EVAL_FLOAT_OPERATION_H

#include "mico/tokens.h"
#include "mico/objects/error.h"
#include "mico/objects/numbers.h"
#include "mico/expressions/expressions.h"
#include "mico/eval/operations/integer.h"

namespace mico { namespace eval {

    template <>
    struct operation<objects::type::FLOAT> {

        using int_type   = objects::derived<objects::type::INTEGER>;
        using value_type = objects::derived<objects::type::FLOAT>;
        using bool_type  = objects::derived<objects::type::BOOLEAN>;
        using error_type = objects::derived<objects::type::FAILURE>;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static const objects::type type_value = objects::type::FLOAT;

        static
        objects::sptr eval_builtin( infix *inf,
                                    objects::sptr obj, objects::sptr call,
                                    environment::sptr env)
        {
            return common_operations::eval_builtin( inf, obj, call, env );
        }

        static
        objects::sptr eval_func( infix *inf,
                                 objects::sptr obj, objects::sptr call,
                                 environment::sptr env)
        {
            return common_operations::eval_func( inf, obj, call, env );
        }

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            auto val = objects::cast_float( obj.get( ) )->value( );
            switch (pref->token( )) {
            case tokens::type::MINUS:
                return value_type::make( -1 * val );
            case tokens::type::BANG:
                return value_type::make( val != 0 );
            default:
                break;
            }
            return error_type::make( pref->pos( ), "Prefix operator '",
                                     pref->token( ), "' is not defined for "
                                                     "floats" );
        }

        static
        objects::sptr eval_float( infix *inf, double lft, double rgh )
        {
            return operation<objects::type::INTEGER>
                            ::eval_float(inf, lft, rgh);
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env )
        {
            auto val = objects::cast<type_value>(obj.get( ))->value( );

            if( (inf->token( ) == tokens::type::LOGIC_AND) && (val == 0)) {
                return bool_type::make( false );
            }
            if( (inf->token( ) == tokens::type::LOGIC_OR) && (val != 0)) {
                return bool_type::make( true );
            }

            objects::sptr right = ev( inf->right( ).get( ) );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }

            switch (right->get_type( )) {
            case objects::type::INTEGER: {
                auto rval = objects::cast_int(right.get( ))->value( );
                auto rdval = static_cast<double>( rval );
                return eval_float( inf, val, rdval );
            }
            case objects::type::FLOAT: {
                auto rval = objects::cast_float(right.get( ))->value( );
                return eval_float(inf, val, rval);
            }
            case objects::type::BOOLEAN: {
                auto rval = objects::cast_bool(right.get( ))->value( );
                return eval_float( inf, val, rval ? 1.0 : 0.0);
            }
            case objects::type::BUILTIN:
                if(inf->token( ) == tokens::type::BIT_OR) {
                    return eval_builtin( inf, obj, right, env);
                }
                break;
            case objects::type::FUNCTION:
                if(inf->token( ) == tokens::type::BIT_OR) {
                    return eval_func( inf, obj, right, env);
                }
                break;
            default:
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

#endif // FLOAT_OPERATION_H
