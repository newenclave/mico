#ifndef MICO_EVAL_STR_OPERATION_H
#define MICO_EVAL_STR_OPERATION_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"
#include "mico/objects/interval.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::STRING> {

        using str_type      = objects::impl<objects::type::STRING>;
        using bool_type     = objects::impl<objects::type::BOOLEAN>;
        using error_type    = objects::impl<objects::type::FAILURE>;
        using int_type      = objects::impl<objects::type::INTEGER>;
        using prefix        = ast::expressions::prefix;
        using infix         = ast::expressions::infix;
        using index         = ast::expressions::index;
        using str_interval  = objects::intervals::string;

        using string_type   = objects::string::value_type;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr /*obj*/ )
        {
            return error_type::make(pref->pos( ), "Prefix operator '",
                                    pref->token( ), "' is not defined for "
                                                    "string");
        }

        static
        objects::sptr eval_str( infix *inf, const string_type &lft,
                                 const string_type &rht )
        {
            switch( inf->token( )) {
            case tokens::type::DOTDOT:
                return  str_interval::make( lft, rht );
            case tokens::type::PLUS:
                return  str_type::make( lft  + rht );
            case tokens::type::EQ:
                return bool_type::make( lft == rht );
            case tokens::type::NOT_EQ:
                return bool_type::make( lft != rht );
            case tokens::type::LT:
                return bool_type::make( lft  < rht );
            case tokens::type::LT_EQ:
                return bool_type::make( lft <= rht );
            case tokens::type::GT:
                return bool_type::make( lft  > rht );
            case tokens::type::GT_EQ:
                return bool_type::make( lft >= rht );
            }

            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "strings");

        }
        static
        objects::sptr eval_int( infix *inf, const string_type &lft,
                                 std::int64_t rht )
        {
            switch( inf->token( )) {
            case tokens::type::ASTERISK: {
                if( rht > 0 ) {
                    std::string res;
                    res.reserve( lft.size( ) * rht );
                    while( rht-- ) {
                        res.insert( res.end( ), lft.begin( ), lft.end( ) );
                    }
                    return str_type::make( std::move(res) );
                } else if( rht == 0  ) {
                    return str_type::make( "" );
                } else {
                    return str_type::make( lft );
                }
            }
            }
            return error_type::make(inf->pos( ), "Infix operation '",
                                    inf->token( ), "' is not defined for "
                                                    "string and integer");

        }

        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::STRING> ref(obj);
            auto str = ref.shared_unref( );

            objects::sptr id = ev( idx->param( ).get( ), env );
            if( id->get_type( ) == objects::type::FAILURE ) {
                return id;
            }

            if( !common::is_numeric( id ) ) {
                return error_type::make( idx->pos( ),
                                         idx->param( ).get( ),
                              " has invalid type; must be integer" );
            }

            std::int64_t index = common::to_index( id );

            auto fix = static_cast<std::size_t>(index);
            if( index < 0 ) {
                fix = str->value( ).size( ) -
                        static_cast<std::size_t>(-1 * index);
            }

            if( fix < str->value( ).size( ) ) {
                return objects::integer::make( str->value( )[fix] );
            } else {
                return error_type::make(
                            idx->param( ).get( ), idx->param( ).get( ),
                            " is not a valid index for the string" );
            }

//            return error_type::make( idx->param( )->pos( ),
//                                     idx->param( ).get( ),
//                          " is not a valid index for the string" );

        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::STRING> ref(obj);
            auto val = ref.unref( );

            objects::sptr right = ev( inf->right( ).get( ), env );
            if( right->get_type( ) == objects::type::FAILURE ) {
                return right;
            }
            switch (right->get_type( )) {
            case objects::type::STRING: {
                auto rght = objects::cast_string(right.get( ));
                return eval_str( inf, val->value( ), rght->value( ) );
            }
            case objects::type::INTEGER: {
                auto rght = objects::cast_int(right.get( ));
                return eval_int( inf, val->value( ), rght->value( ) );
            }
            default:
                return common::common_infix( inf, obj, right, env );
            }
        }
    };

    using string = operation<objects::type::STRING>;

}}}

#endif // STR_OPERATION_H
