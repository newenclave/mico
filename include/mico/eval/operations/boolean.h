#ifndef MICO_EVAL_BOOL_OPERATION_H
#define MICO_EVAL_BOOL_OPERATION_H

#include "mico/eval/operation.h"
#include "mico/tokens.h"
#include "mico/expressions.h"
#include "mico/eval/operations/float.h"
#include "mico/eval/operations/integer.h"
#include "mico/eval/operations/common.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::BOOLEAN> {
        using float_type = objects::impl<objects::type::FLOAT>;
        using int_type   = objects::impl<objects::type::INTEGER>;
        using error_type = objects::impl<objects::type::FAILURE>;
        using bool_type  = objects::impl<objects::type::BOOLEAN>;
        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            common::reference<objects::type::BOOLEAN> ref(obj);
            auto val = ref.unref( )->value( );

            if( pref->token( ) == tokens::type::BANG ) {
                return bool_type::make( !val );
            }

            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "booleans");
        }

        static
        objects::sptr eval_float( infix *inf, double lft, double rht)
        {
            return operation<objects::type::INTEGER>::eval_float(inf, lft, rht);
        }

        static
        objects::sptr eval_int( infix *inf, std::int64_t lft, std::int64_t rht)
        {
            return operation<objects::type::INTEGER>::eval_int(inf, lft, rht);
        }

        static
        objects::sptr eval_bool( infix *inf, bool lft, bool rht)
        {
            switch (inf->token( )) {
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
                                                    "booleans");
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env )
        {
            common::reference<objects::type::BOOLEAN> ref(obj);
            auto val = ref.unref( )->value( );

            if( (inf->token( ) == tokens::type::LOGIC_AND) && !val ) {
                return bool_type::make( false );
            }

            if( (inf->token( ) == tokens::type::LOGIC_OR) && val ) {
                return bool_type::make( true );
            }

            objects::sptr right = ev( inf->right( ).get( ) );
            if( common::is_fail( right ) ) {
                return right;
            }

            switch (right->get_type( )) {
            case objects::type::INTEGER: {
                auto rval = objects::cast_int(right.get( ))->value( );
                return eval_int(inf, val ? 1 : 0, rval);
            }
            case objects::type::FLOAT: {
                auto rval = objects::cast_float(right.get( ))->value( );
                return eval_float(inf, val ? 1.0 : 0.0, rval);
            }
            case objects::type::BOOLEAN: {
                auto rval = objects::cast_bool(right.get( ))->value( );
                return eval_bool(inf, val, rval);
            }
            default:
                return common::common_infix( inf, obj, right, env );
            }

            return error_type::make(inf->pos( ), "Infix operation ",
                                    obj->get_type( )," '",
                                    inf->token( ), "' ",
                                    right->get_type( ),
                                    " is not defined");

        }
    };

    using boolean = operation<objects::type::BOOLEAN>;

}}}

#endif // BOOL_OPERATION_H
