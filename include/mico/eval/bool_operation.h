#ifndef MICO_EVAL_BOOL_OPERATION_H
#define MICO_EVAL_BOOL_OPERATION_H

#include "mico/eval/operation.h"
#include "mico/objects/numbers.h"
#include "mico/tokens.h"
#include "mico/objects/error.h"
#include "mico/expressions/expressions.h"
#include "mico/eval/float_operation.h"
#include "mico/eval/int_operation.h"

namespace mico { namespace eval {

    template <>
    struct operation<objects::type::BOOLEAN> {
        using float_type = objects::derived<objects::type::FLOAT>;
        using int_type   = objects::derived<objects::type::INTEGER>;
        using error_type = objects::derived<objects::type::FAILURE>;
        using bool_type  = objects::derived<objects::type::BOOLEAN>;
        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            auto val = static_cast<bool_type *>( obj.get( ) )->value( );

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
        objects::sptr eval_infix( infix *inf, objects::sptr obj, eval_call ev )
        {
            auto val = static_cast<bool_type *>(obj.get( ))->value( );

            if( (inf->token( ) == tokens::type::LOGIC_AND) && !val ) {
                return bool_type::make( false );
            }

            if( (inf->token( ) == tokens::type::LOGIC_OR) && val ) {
                return bool_type::make( true );
            }

            objects::sptr right = ev( inf->right( ).get( ) );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }

            switch (right->get_type( )) {
            case objects::type::INTEGER: {
                auto rval = static_cast<int_type *>(right.get( ))->value( );
                return eval_int(inf, val ? 1 : 0, rval);
            }
            case objects::type::FLOAT: {
                auto rval = static_cast<float_type *>(right.get( ))->value( );
                return eval_float(inf, val ? 1.0 : 0.0, rval);
            }
            case objects::type::BOOLEAN: {
                auto rval = static_cast<bool_type *>(right.get( ))->value( );
                return eval_bool(inf, val, rval);
            }
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

#endif // BOOL_OPERATION_H
