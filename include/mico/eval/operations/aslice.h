#ifndef MICO_EVAL_STR_ASLICE_H
#define MICO_EVAL_STR_ASLICE_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"
#include "mico/objects/interval.h"

namespace mico { namespace eval { namespace operations {


    template <>
    struct operation<objects::type::ASLICE> {

        using index         = ast::expressions::index;
        using error_type    = objects::impl<objects::type::FAILURE>;
        using int_type      = objects::impl<objects::type::INTEGER>;
        using infix         = ast::expressions::infix;

        static
        objects::sptr eval_ival_index( index *idx,
                                       objects::aslice::sptr str,
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

            auto res = objects::aslice::make( str->value( ),
                                              str->fix_id(start),
                                              str->fix_id(stop) );

            return res;
        }


        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::ASLICE> ref(obj);
            auto str = ref.shared_unref( );

            objects::sptr id = ev( idx->param( ).get( ), env );
            if( id->get_type( ) == objects::type::FAILURE ) {
                return id;
            }

            if( id->get_type( )  == objects::type::INTERVAL ) {
                return eval_ival_index( idx, str, id );
            }

            if( !common::is_numeric( id ) ) {
                return error_type::make( idx->param( )->pos( ),
                                         idx->param( ).get( ),
                              " has invalid type; must be numeric" );
            }

            std::int64_t index = common::to_index( id );

            if( str->valid_id( index ) ) {
                return str->at( index );
            } else {
                return error_type::make(
                            idx->param( )->pos( ), idx->param( ).get( ),
                            " is not a valid index for the slice" );
            }
        }

    };

    using sslice = operation<objects::type::SSLICE>;

}}}

#endif // SSLICE_H
