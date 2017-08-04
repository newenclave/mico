#ifndef MICO_EVAL_OPERATIONS_INFINITE_H
#define MICO_EVAL_OPERATIONS_INFINITE_H

#include "mico/tokens.h"
#include "mico/objects/error.h"
#include "mico/objects/numbers.h"
#include "mico/expressions.h"
#include "mico/eval/operations/integer.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::INF_OBJ> {

        using value_type = objects::impl<objects::type::INF_OBJ>;

        using int_type   = objects::impl<objects::type::INTEGER>;
        using bool_type  = objects::impl<objects::type::BOOLEAN>;
        using error_type = objects::impl<objects::type::FAILURE>;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            common::reference<objects::type::INF_OBJ> ref(obj);
            auto val = ref.unref( );

            switch (pref->token( )) {
            case tokens::type::MINUS:
                return value_type::make( !val->is_negative( ) );
            default:
                break;
            }
            return error_type::make( pref->pos( ), "Prefix operator '",
                                     pref->token( ), "' is not defined for "
                                                     "infinite" );
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env )
        {
            return common::eval_infix( inf, obj, ev, env );
        }
    };

}}}

#endif // INFINITE_H
