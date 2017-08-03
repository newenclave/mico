#ifndef MICO_EVAL_SLICES_H
#define MICO_EVAL_SLICES_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"
#include "mico/objects/interval.h"
#include "mico/objects/slices.h"

namespace mico { namespace eval { namespace operations {

    template <objects::type TN>
    struct operation_slice {

        using index         = ast::expressions::index;
        using error_type    = objects::impl<objects::type::FAILURE>;
        using int_type      = objects::impl<objects::type::INTEGER>;
        using infix         = ast::expressions::infix;
        using object_type   = objects::impl<TN>;
        using value_type    = typename object_type::sptr;

        static
        objects::sptr eval_ival_index( index *idx,
                                       value_type str,
                                       objects::sptr id )
        {
            return common::eval_ival_index<object_type>(idx, str, id);
        }

        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<TN> ref(obj);
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

    template <>
    struct operation<objects::type::ASLICE> {

        using index         = ast::expressions::index;
        using error_type    = objects::impl<objects::type::FAILURE>;
        using int_type      = objects::impl<objects::type::INTEGER>;
        using infix         = ast::expressions::infix;
        using parent_type   = operation_slice<objects::type::ASLICE>;

        static
        objects::sptr eval_ival_index( index *idx,
                                       objects::aslice::sptr str,
                                       objects::sptr id )
        {
            return parent_type::eval_ival_index(idx, str, id);
        }

        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            return parent_type::eval_index( idx, obj, ev, env );
        }
    };

    template <>
    struct operation<objects::type::SSLICE> {

        using index         = ast::expressions::index;
        using error_type    = objects::impl<objects::type::FAILURE>;
        using int_type      = objects::impl<objects::type::INTEGER>;
        using infix         = ast::expressions::infix;
        using parent_type   = operation_slice<objects::type::SSLICE>;

        static
        objects::sptr eval_ival_index( index *idx,
                                       objects::sslice::sptr str,
                                       objects::sptr id )
        {
            return parent_type::eval_ival_index( idx, str, id );
        }

        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            return parent_type::eval_index( idx, obj, ev, env );
        }
    };

    using sslice = operation<objects::type::SSLICE>;
    using aslice = operation<objects::type::ASLICE>;

}}}

#endif // SSLICE_H
