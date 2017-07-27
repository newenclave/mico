#ifndef MICO_EVAL_MODULE_H
#define MICO_EVAL_MODULE_H

#include "mico/tokens.h"
#include "mico/expressions/infix.h"
#include "mico/expressions/prefix.h"
#include "mico/eval/operations/common.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::MODULE> {

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;
        using index  = ast::expressions::index;

//        static
//        objects::sptr eval_prefix( tokens::type, objects::sptr )
//        {

//        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr /*env*/ )
        {
            common::reference<objects::type::MODULE> ref(obj);
            auto mod = ref.shared_unref( );

            if( inf->token( ) == tokens::type::DOT ) {
                if( inf->right( )->get_type( ) == ast::type::IDENT ) {

                    auto id = inf->right( )->str( );

                    if( auto e = mod->env( ) ) {
                        auto val = e->data( ).find( id );
                        if( val != e->data( ).end( ) ) {
                            return val->second;
                        } else {
                            return common::error_type::make(
                                        inf->right( )->pos( ),
                                        "Identifier not found '", id, "'");
                        }
                    }

                } else if( inf->right( )->get_type( ) == ast::type::CALL ) {
                    //// very special case
                    return ev( inf->right( ).get( ), mod->env( ) );

                } else {
                    return common::error_type::make( inf->right( )->pos( ),
                                                     "Bad ident for module ",
                                                     inf->right( )->str( ));
                }
            }

            return common::error_type::make( inf->pos( ), "Infix operation ",
                                            inf->token( ), "' ",
                                            " is not defined for modules");
        }

    };

}}}

#endif // MODULE_H
