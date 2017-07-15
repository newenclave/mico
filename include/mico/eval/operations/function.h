#ifndef MICO_EVAL_OBJECTS_FUNCTION_H
#define MICO_EVAL_OBJECTS_FUNCTION_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::FUNCTION> {
        using error_type = objects::derived<objects::type::FAILURE>;
        using prefix     = ast::expressions::prefix;
        using infix      = ast::expressions::infix;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr /*obj*/ )
        {
            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "function");
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::STRING> ref(obj);
            obj = ref.shared_unref( );

            objects::sptr right = ev( inf->right( ).get( ) );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }
            return common::common_infix( inf, obj, right, env );
        }
    };

}}}


#endif // FUNCTION_H
