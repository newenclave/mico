#ifndef MICO_EVAL_INT_OPRATIONS_H
#define MICO_EVAL_INT_OPRATIONS_H

#include "mico/eval/operation.h"
#include "mico/tokens.h"
#include "mico/eval/operations/common_operations.h"

#include "mico/ast.h"

namespace mico { namespace eval {

    template <>
    struct operation<objects::type::INTEGER> {

        using float_type    = objects::derived<objects::type::FLOAT>;
        using value_type    = objects::derived<objects::type::INTEGER>;
        using error_type    = objects::derived<objects::type::FAILURE>;
        using bool_type     = objects::derived<objects::type::BOOLEAN>;
        using builtin_type  = objects::derived<objects::type::BUILTIN>;
        using function_type = objects::derived<objects::type::FUNCTION>;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static const objects::type type_value = objects::type::INTEGER;
        static const objects::type float_type_value = objects::type::FLOAT;
        static const objects::type bool_type_value = objects::type::BOOLEAN;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            auto val = objects::cast_int( obj.get( ) )->value( );
            auto uval = static_cast<std::uint64_t>(val);

            switch (pref->token( )) {
            case tokens::type::MINUS:
                return value_type::make( -1 * val );
            case tokens::type::TILDA:
                return value_type::make( ~uval );
            case tokens::type::BANG:
                return value_type::make( uval != 0 );
            default:
                break;
            }
            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "integers");
        }

        static
        objects::sptr eval_float( infix *inf, double lft, double rht )
        {
            switch (inf->token( )) {
            case tokens::type::MINUS:
                return float_type::make( lft  - rht );
            case tokens::type::PLUS:
                return float_type::make( lft  + rht );
            case tokens::type::ASTERISK:
                return float_type::make( lft  * rht );
            case tokens::type::SLASH:
                if( rht != 0 ) {
                    return float_type::make( lft  / rht );
                } else {
                    return error_type::make( inf->pos( ),
                                             "Division by zero. '/'" );
                }

            case tokens::type::LOGIC_AND:
                return  bool_type::make( lft && rht );
            case tokens::type::LOGIC_OR:
                return  bool_type::make( lft || rht );
            case tokens::type::GT:
                return  bool_type::make( lft  > rht );
            case tokens::type::LT:
                return  bool_type::make( lft  < rht );
            case tokens::type::GT_EQ:
                return  bool_type::make( lft >= rht );
            case tokens::type::LT_EQ:
                return  bool_type::make( lft <= rht );
            case tokens::type::EQ:
                return  bool_type::make( lft == rht );
            case tokens::type::NOT_EQ:
                return  bool_type::make( lft != rht );
            default:
                break;
            }
            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "float");
        }

        static
        objects::sptr eval_int( infix *inf, std::int64_t lft, std::int64_t rht )
        {
            auto ulft = static_cast<std::uint64_t>(lft);
            auto urgt = static_cast<std::uint64_t>(rht);

            switch (inf->token( )) {
            case tokens::type::MINUS:
                return value_type::make( lft  - rht );
            case tokens::type::PLUS:
                return value_type::make( lft  + rht );
            case tokens::type::ASTERISK:
                return value_type::make( lft  * rht );
            case tokens::type::SLASH:
                if( rht != 0 ) {
                    return value_type::make( lft  / rht );
                } else {
                    return error_type::make( inf->pos( ),
                                             "Division by zero. '/'" );
                }
            case tokens::type::PERCENT:
                if( rht != 0 ) {
                    return value_type::make( lft  % rht );
                } else {
                    return error_type::make( inf->pos( ),
                                             "Division by zero. '%'" );
                }
            case tokens::type::SHIFT_LEFT:
                return  value_type::make( ulft << urgt );
            case tokens::type::SHIFT_RIGHT:
                return  value_type::make( ulft >> urgt );

            case tokens::type::GT:
                return  bool_type::make( lft  > rht );
            case tokens::type::LT:
                return  bool_type::make( lft  < rht );
            case tokens::type::GT_EQ:
                return  bool_type::make( lft >= rht );
            case tokens::type::LT_EQ:
                return  bool_type::make( lft <= rht );
            case tokens::type::EQ:
                return  bool_type::make( lft == rht );
            case tokens::type::NOT_EQ:
                return  bool_type::make( lft != rht );
            case tokens::type::LOGIC_AND:
                return  bool_type::make( ulft && urgt );
            case tokens::type::LOGIC_OR:
                return  bool_type::make( ulft || urgt );

            case tokens::type::BIT_AND:
                return  value_type::make( ulft & urgt );
            case tokens::type::BIT_OR:
                return  value_type::make( ulft | urgt );
            case tokens::type::BIT_XOR:
                return  value_type::make( ulft ^ urgt );
            default:
                break;
            }
            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "integers");
        }

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
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            auto val = objects::cast_int(obj.get( ))->value( );

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
                return eval_int(inf, val, rval);
            }
            case objects::type::FLOAT: {
                auto lval = static_cast<double>(val);
                auto rval = objects::cast_float(right.get( ))->value( );
                return eval_float(inf, lval, rval);
            }
            case objects::type::BOOLEAN: {
                auto rval = objects::cast_bool(right.get( ))->value( );
                return eval_int(inf, val, rval ? 1 : 0);
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


#endif // INT_OPRATIONS_H
