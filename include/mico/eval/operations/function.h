#ifndef MICO_EVAL_OBJECTS_FUNCTION_H
#define MICO_EVAL_OBJECTS_FUNCTION_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::FUNCTION> {
        using error_type = objects::impl<objects::type::FAILURE>;
        using prefix     = ast::expressions::prefix;
        using infix      = ast::expressions::infix;
        using call_type  = ast::expressions::call;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            if( pref->token( ) == tokens::type::ASTERISK ) {
                auto unref = objects::reference::unref( obj );
                if( common::is_func( unref ) ) {
                    auto func = objects::cast_func( unref );
                    if( func->is_part( ) ) {
                        return objects::function::make_from_partial( func );
                    }
                }
                return obj;
            }
            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "function");
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            obj = objects::reference::unref( obj );

            if( inf->token( ) == tokens::type::BIT_OR ) {
                objects::sptr right = ev( inf->right( ).get( ), env );
                if( common::is_fail( right ) ) {
                    return right;
                }
                return common::common_infix( inf, obj, right, env );
            }
            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                   "functions");
        }
    };

    using function = operation<objects::type::FUNCTION>;

}}}


#endif // FUNCTION_H
