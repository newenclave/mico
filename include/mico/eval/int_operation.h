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

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;



        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            auto val = static_cast<value_type *>( obj.get( ) );
            auto uval = static_cast<std::uint64_t>(val);
            switch (pref->token( )) {
            case tokens::type::MINUS:
                return value_type::make( -1 * val->value( ) );
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

        float_type::sptr eval_as_float( tokens::type tt,
                                        objects::sptr lft, objects::sptr rgt )
        {
            double lftv = static_cast<float_type *>(lft.get( ));
            double rgtv = static_cast<float_type *>(rgt.get( ));

            switch (tt) {
            case tokens::type::MINUS:
                return float_type::make( lftv->value( ) - rgtv->value( ) );
            case tokens::type::PLUS:
                return float_type::make( lftv->value( ) + rgtv->value( ) );
            case tokens::type::ASTERISK:
                return float_type::make( lftv->value( ) * rgtv->value( ) );
            case tokens::type::SLASH:
                return float_type::make( lftv->value( ) / rgtv->value( ) );
            default:
                break;
            }
        }

        objects::sptr eval_infix( infix *inf, objects::sptr, eval_call )
        {
            auto val = static_cast<value_type *>( obj.get( ) );
            auto uval = static_cast<std::uint64_t>(val);

            switch (inf->token( )) {


            }

            return error_type::make(pref->pos( ), "Infix pperation '",
                                    pref->token( ), "' is not defined for "
                                                    "integers");
        }

    };

}}


#endif // INT_OPRATIONS_H
