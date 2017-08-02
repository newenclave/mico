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
        objects::sptr eval_ival_index( index *idx,
                                       objects::array::sptr str,
                                       objects::sptr id )
        {
            auto ival = objects::cast_ival( id.get( ) );

            if( !common::is_numeric( ival->domain( )) ) {
                return error_type::make( idx->param( )->pos( ),
                                         idx->param( ).get( ),
                              " has invalid type; must be numeric" );
            }

            std::int64_t start = common::to_index( ival->begin( ) );
            std::int64_t stop  = common::to_index( ival->end( ) );

            if( !str->valid_id( start ) || !str->valid_id( stop ) ) {
                return error_type::make( idx->param( )->pos( ),
                                         idx->param( ).get( ),
                                         " has invalid range" );
            }

            auto res = objects::aslice::make( str,
                                              str->fix_id(start),
                                              str->fix_id(stop) );

            return res;
        }

        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::ARRAY> ref(obj);
            auto arr = ref.shared_unref( );

            objects::sptr id = ev( idx->param( ).get( ), env );
            if( id->get_type( ) == objects::type::FAILURE ) {
                return id;
            }

            if( id->get_type( )  == objects::type::INTERVAL ) {
                return eval_ival_index( idx, arr, id );
            }

            if( !common::is_numeric( id ) ) {
                return error_type::make( idx->pos( ), idx->param( ).get( ),
                                         " has invalid type; must be integer" );
            }

            std::int64_t index = common::to_index( id );

            if( auto res = arr->at( index ) ) {
                return res;
            }

            return error_type::make( idx->param( )->pos( ),
                                     idx->param( ).get( ),
                          " is not a valid index for the array" );
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::ARRAY> ref(obj);
            obj = ref.shared_unref( );

            objects::sptr right = ev( inf->right( ).get( ), env );
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

    using array = operation<objects::type::ARRAY>;

}}}

#endif // ARRAYS_H
