#ifndef MICO_EVAL_INT_OPRATIONS_H
#define MICO_EVAL_INT_OPRATIONS_H

#include "mico/eval/operation.h"
#include "mico/objects/numbers.h"
#include "mico/objects/null.h"
#include "mico/tokens.h"
#include "mico/objects/error.h"
#include "mico/expressions/expressions.h"

namespace mico { namespace eval {

    template <>
    struct operation<objects::type::INTEGER> {

        using float_type = objects::derived<objects::type::FLOAT>;
        using value_type = objects::derived<objects::type::INTEGER>;
        using error_type = objects::derived<objects::type::FAILURE>;
        using bool_type  = objects::derived<objects::type::BOOLEAN>;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;



        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            auto val = static_cast<value_type *>( obj.get( ) )->value( );
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

        objects::sptr eval_as_float( infix *inf,
                                     objects::sptr lft, objects::sptr rgt )
        {
            double lftv = static_cast<value_type *>(lft.get( ))->value( );
            double rgtv = static_cast<float_type *>(rgt.get( ))->value( );

            switch (inf->token( )) {
            case tokens::type::MINUS:
                return float_type::make( lftv  - rgtv );
            case tokens::type::PLUS:
                return float_type::make( lftv  + rgtv );
            case tokens::type::ASTERISK:
                return float_type::make( lftv  * rgtv );
            case tokens::type::SLASH:
                return float_type::make( lftv  / rgtv );
            case tokens::type::GT:
                return  bool_type::make( lftv  > rgtv );
            case tokens::type::LT:
                return  bool_type::make( lftv  < rgtv );
            case tokens::type::GT_EQ:
                return  bool_type::make( lftv >= rgtv );
            case tokens::type::LT_EQ:
                return  bool_type::make( lftv <= rgtv );
            case tokens::type::EQ:
                return  bool_type::make( lftv == rgtv );
            case tokens::type::NOT_EQ:
                return  bool_type::make( lftv != rgtv );
            default:
                break;
            }
            return error_type::make(inf->pos( ), "Infix pperation '",
                                    inf->token( ), "' is not defined for "
                                                    "float");
        }

        objects::sptr eval_as_int( infix *inf,
                                   objects::sptr lft, objects::sptr rgt )
        {
            auto lftv = static_cast<value_type *>(lft.get( ))->value( );
            auto rgtv = static_cast<value_type *>(rgt.get( ))->value( );

            auto ulftv = lftv;
            auto urgtv = rgtv;

            switch (inf->token( )) {
            case tokens::type::MINUS:
                return value_type::make( lftv  - rgtv );
            case tokens::type::PLUS:
                return value_type::make( lftv  + rgtv );
            case tokens::type::ASTERISK:
                return value_type::make( lftv  * rgtv );
            case tokens::type::SLASH:
                return value_type::make( lftv  / rgtv );
            case tokens::type::PERCENT:
                return value_type::make( lftv  % rgtv );
            case tokens::type::GT:
                return  bool_type::make( lftv  > rgtv );
            case tokens::type::LT:
                return  bool_type::make( lftv  < rgtv );
            case tokens::type::GT_EQ:
                return  bool_type::make( lftv >= rgtv );
            case tokens::type::LT_EQ:
                return  bool_type::make( lftv <= rgtv );
            case tokens::type::EQ:
                return  bool_type::make( lftv == rgtv );
            case tokens::type::NOT_EQ:
                return  bool_type::make( lftv != rgtv );

            case tokens::type::BIT_AND:
                return  value_type::make( ulftv & urgtv );
            case tokens::type::BIT_OR:
                return  value_type::make( ulftv | urgtv );
            case tokens::type::BIT_XOR:
                return  value_type::make( ulftv ^ urgtv );
            default:
                break;
            }
            return error_type::make(inf->pos( ), "Infix pperation '",
                                    inf->token( ), "' is not defined for "
                                                    "integers");
        }

        objects::sptr eval_as_bool( infix *inf,
                                    objects::sptr lft, objects::sptr rgt )
        {
            return error_type::make(inf->pos( ), "Infix pperation '",
                                    inf->token( ), "' is not defined for "
                                                    "boolean");
        }

        objects::sptr eval_infix( infix *inf, objects::sptr obj, eval_call )
        {
            auto val = static_cast<value_type *>(obj.get( ))->value( );
            auto uval = static_cast<std::uint64_t>(val);

            return error_type::make(inf->pos( ), "Infix pperation '",
                                    inf->token( ), "' is not defined for "
                                                    "integers");
        }

    };

}}


#endif // INT_OPRATIONS_H
