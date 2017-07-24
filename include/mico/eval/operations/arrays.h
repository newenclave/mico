#ifndef MICO_EVAL_ARRAYS_H
#define MICO_EVAL_ARRAYS_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::ARRAY> {

        using error_type = objects::impl<objects::type::FAILURE>;
        using int_type   = objects::impl<objects::type::INTEGER>;
        using prefix     = ast::expressions::prefix;
        using infix      = ast::expressions::infix;
        using index      = ast::expressions::index;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            common::reference<objects::type::ARRAY> ref(obj);
            auto tt = ref.unref( );

            if( pref->token( ) == tokens::type::ASTERISK ) {
                return int_type::make( tt->value( ).size( ) );
            }
            return error_type::make( pref->pos( ),   "Prefix operator '",
                                     pref->token( ), "' is not defined for"
                                                     " arrays");
        }

        static
        objects::sptr eval_array( environment::sptr env,
                                  objects::sptr lft, objects::sptr rght )
        {
            auto ltable = objects::cast_array(lft);
            auto rtable = objects::cast_array(rght);

            auto res = ltable->clone( );
            auto restabe = objects::cast_array(res);

            for( auto &v: rtable->value( ) ) {
                restabe->push( env.get( ), v->value( ) );
            }

            return res;
        }

        static
        objects::sptr eval_index( index *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr /*env*/  )
        {

            common::reference<objects::type::ARRAY> ref(obj);
            auto arr = ref.shared_unref( );

            objects::sptr id = ev( inf->param( ).get( ) );
            if( id->get_type( ) == objects::type::FAILURE ) {
                return id;
            }

            std::int64_t index = std::numeric_limits<std::int64_t>::max( );

            if( id->get_type( ) == objects::type::INTEGER ) {
                auto iid = objects::cast_int( id.get( ) );
                index = iid->value( );
            } else if( id->get_type( ) == objects::type::FLOAT ) {
                auto iid = objects::cast_float( id.get( ) );
                index = static_cast<decltype(index)>(iid->value( ));
            } else {
                return error_type::make( inf->pos( ),
                                         inf->param( ).get( ),
                              " has invalid type; must be integer" );
            }

            if( auto res = arr->at( index ) ) {
                return res;
            }

            return error_type::make( inf->param( )->pos( ),
                                     inf->param( ).get( ),
                          " is not a valid index for the array" );
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::ARRAY> ref(obj);
            obj = ref.shared_unref( );

            objects::sptr right = ev( inf->right( ).get( ) );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }

            if( (inf->token( ) == tokens::type::EQ) ||
                (inf->token( ) == tokens::type::NOT_EQ) ) {
                return common::eval_equal( inf, obj, right );
            }

            switch (right->get_type( )) {
            case objects::type::ARRAY:
                if( inf->token( ) == tokens::type::PLUS ) {
                    return eval_array( env, obj, right );
                }
                break;
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

}}}

#endif // ARRAYS_H
