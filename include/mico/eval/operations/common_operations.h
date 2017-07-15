#ifndef MICO_EVAL_COMMON_OPERATIONS_H
#define MICO_EVAL_COMMON_OPERATIONS_H

#include "mico/eval/operation.h"
#include "mico/tokens.h"
#include "mico/objects.h"
#include "mico/expressions/expressions.h"

namespace mico { namespace eval {

    struct common_operations {

        using float_type    = objects::floating;
        using value_type    = objects::integer;
        using error_type    = objects::error;
        using bool_type     = objects::boolean;
        using builtin_type  = objects::builtin;
        using function_type = objects::function;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        static
        objects::sptr eval_builtin( infix * /*inf*/,
                                    objects::sptr obj, objects::sptr call,
                                    environment::sptr /*env*/ )
        {
            objects::slist par { obj };
            auto func = objects::cast_builtin(call.get( ));
            auto call_env = environment::make( func->env( ) );
            return objects::tail_call::make( call, std::move(par), call_env );
        }

        static
        objects::sptr eval_func( infix *inf,
                                 objects::sptr obj, objects::sptr call,
                                 environment::sptr /*env*/ )
        {
            auto func = objects::cast_func(call.get( ));
            auto call_env = environment::make( func->env( ) );
            if( func->params( ).size( ) != 1 ) {
                return error_type::make(inf->pos( ),
                                        "Invalid parameters count. ",
                                        "Must be 1");
            }

            for( auto &p: func->params( ) ) {
                if( p->get_type( ) != ast::type::IDENT ) {
                    return error_type::make( inf->pos( ),
                                             "Invalid parameter type.",
                                             p->get_type( ));
                }
                call_env->set( p->str( ), obj );
            }
            return objects::tail_call::make( call, call_env );
        }

    };


}}

#endif // COMMON_OPERATIONS_H
