#ifndef MICO_EVAL_CHAR_OPRATIONS_H
#define MICO_EVAL_CHAR_OPRATIONS_H

#include "mico/eval/operation.h"
#include "mico/tokens.h"
#include "mico/eval/operations/common.h"

#include "mico/objects/interval.h"
#include "mico/ast.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::CHARACTER> {

        using float_type     = objects::impl<objects::type::FLOAT>;
        using int_type       = objects::impl<objects::type::INTEGER>;
        using value_type     = objects::impl<objects::type::CHARACTER>;
        using error_type     = objects::impl<objects::type::FAILURE>;
        using bool_type      = objects::impl<objects::type::BOOLEAN>;
        using builtin_type   = objects::impl<objects::type::BUILTIN>;
        using function_type  = objects::impl<objects::type::FUNCTION>;
        using int_interval   = objects::intervals::integer;
        using char_interval  = objects::intervals::character;
        using float_interval = objects::intervals::floating;


        using symbol_type = mico::string::value_type;
        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static const objects::type type_value = objects::type::INTEGER;
        static const objects::type float_type_value = objects::type::FLOAT;
        static const objects::type bool_type_value = objects::type::BOOLEAN;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            common::reference<objects::type::CHARACTER> ref(obj);

            auto val = ref.unref( );
            auto uval = static_cast<std::uint64_t>(val->value( ));

            switch (pref->token( )) {
            case tokens::type::MINUS:
                return value_type::make( -1 * val->value( ) );
            case tokens::type::TILDA:
                return value_type::make( ~uval );
            case tokens::type::BANG:
                return bool_type::make( uval != 0 );
            default:
                break;
            }
            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "integers");
        }

        static
        objects::sptr eval_int( infix *inf, symbol_type lft, std::int64_t rht )
        {
            auto urgt = static_cast<symbol_type>(rht);

            switch (inf->token( )) {
            case tokens::type::DOTDOT:
                return char_interval::make( lft, urgt );
            case tokens::type::MINUS:
                return value_type::make( lft  - urgt );
            case tokens::type::PLUS:
                return value_type::make( lft  + urgt );
            case tokens::type::ASTERISK:
                return value_type::make( lft  * urgt );
            case tokens::type::SLASH:
                if( rht != 0 ) {
                    return value_type::make( lft  / urgt );
                } else {
                    return error_type::make( inf->pos( ),
                                             "Division by zero. '/'" );
                }
            case tokens::type::PERCENT:
                if( rht != 0 ) {
                    return value_type::make( lft  % urgt );
                } else {
                    return error_type::make( inf->pos( ),
                                             "Division by zero. '%'" );
                }
            case tokens::type::SHIFT_LEFT:
                return  value_type::make( lft << urgt );
            case tokens::type::SHIFT_RIGHT:
                return  value_type::make( lft >> urgt );

            case tokens::type::GT:
                return  bool_type::make( lft  > urgt );
            case tokens::type::LT:
                return  bool_type::make( lft  < urgt );
            case tokens::type::GT_EQ:
                return  bool_type::make( lft >= urgt );
            case tokens::type::LT_EQ:
                return  bool_type::make( lft <= urgt );
            case tokens::type::EQ:
                return  bool_type::make( lft == urgt );
            case tokens::type::NOT_EQ:
                return  bool_type::make( lft != urgt );
            case tokens::type::LOGIC_AND:
                return  bool_type::make( lft && urgt );
            case tokens::type::LOGIC_OR:
                return  bool_type::make( lft || urgt );

            case tokens::type::BIT_AND:
                return  value_type::make( lft & urgt );
            case tokens::type::BIT_OR:
                return  value_type::make( lft | urgt );
            case tokens::type::BIT_XOR:
                return  value_type::make( lft ^ urgt );
            default:
                break;
            }
            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "characters");
        }

        static
        objects::sptr eval_builtin( infix *inf,
                                    objects::sptr obj, objects::sptr call,
                                    environment::sptr env)
        {
            return common::eval_builtin( inf, obj, call, env );
        }

        static
        objects::sptr eval_func( infix *inf,
                                 objects::sptr obj, objects::sptr call,
                                 environment::sptr env)
        {
            return common::eval_func( inf, obj, call, env );
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::CHARACTER> ref(obj);
            auto val = ref.unref( )->value( );

            if( (inf->token( ) == tokens::type::LOGIC_AND) && (val == 0)) {
                return bool_type::make( false );
            }
            if( (inf->token( ) == tokens::type::LOGIC_OR) && (val != 0)) {
                return bool_type::make( true );
            }

            objects::sptr right = ev( inf->right( ).get( ), env );
            if( common::is_fail( right ) ) {
                return right;
            }

            switch (right->get_type( )) {
            case objects::type::INTEGER: {
                auto rval = objects::cast_int(right.get( ))->value( );
                return eval_int(inf, val, rval);
            }
            case objects::type::FLOAT: {
                auto rval = objects::cast_float(right.get( ))->value( );
                return eval_int(inf, val, static_cast<std::int64_t>(rval) );
            }
            case objects::type::BOOLEAN: {
                auto rval = objects::cast_bool(right.get( ))->value( );
                return eval_int(inf, val, rval ? 1 : 0);
            }
            default:
                return common::common_infix( inf, obj, right, env );
            }
        }

    };

    using integer = operation<objects::type::INTEGER>;

}}}


#endif // INT_OPRATIONS_H
