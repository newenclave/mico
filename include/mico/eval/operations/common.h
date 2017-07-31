#ifndef MICO_EVAL_COMMON_OPERATIONS_H
#define MICO_EVAL_COMMON_OPERATIONS_H

#include "mico/eval/operation.h"
#include "mico/tokens.h"
#include "mico/objects.h"
#include "mico/expressions.h"

namespace mico { namespace eval { namespace operations {

    struct common {

        using float_type    = objects::floating;
        using value_type    = objects::integer;
        using error_type    = objects::error;
        using bool_type     = objects::boolean;
        using builtin_type  = objects::builtin;
        using function_type = objects::function;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;
        using index  = ast::expressions::index;

        template<objects::type T>
        struct reference {

            using derive_type = objects::impl<T>;
            using shared_derive = std::shared_ptr<derive_type>;

            explicit
            reference( objects::sptr o )
                :value_(o)
            { }

            objects::base *raw( )
            {
                return value_.get( );
            }

            objects::sptr shared_raw( )
            {
                return value_;
            }

            derive_type *unref( )
            {
                if( value_->get_type(  ) == objects::type::REFERENCE ) {
                    auto rw = objects::cast<objects::type::REFERENCE>(raw( ));
                    return objects::cast<T>( rw->value( ).get( ) );
                } else  {
                    return objects::cast<T>( value_.get( ) );
                }
            }

            shared_derive shared_unref( )
            {
                if( value_->get_type(  ) == objects::type::REFERENCE ) {
                    auto rw = objects::cast_ref( value_ );
                    return objects::cast<T>( rw->value( ) );
                } else  {
                    return objects::cast<T>( value_ );
                }
            }

        private:
            objects::sptr value_;
        };


        static
        bool is_fail( const objects::base *obj )
        {
            return obj->get_type( ) == objects::type::FAILURE;
        }

        static
        bool is_fail( const objects::sptr &obj )
        {
            return is_fail( obj.get( ) );
        }

        static
        bool is_func( const objects::base *obj )
        {
            return obj->get_type( ) == objects::type::FUNCTION;
        }

        static
        bool is_func( const objects::sptr &obj )
        {
            return is_func( obj.get( ) );
        }

        static
        bool is_ident( const ast::node *obj )
        {
            return obj->get_type( ) == ast::type::IDENT;
        }

        static
        bool is_ident( const ast::node::uptr &obj )
        {
            return is_ident( obj.get( ) );
        }

        static
        bool is_numeric( const objects::base *obj )
        {
            return ( obj->get_type( ) == objects::type::INTEGER )
                || ( obj->get_type( ) == objects::type::FLOAT )
                 ;
        }

        static
        bool is_numeric( const objects::sptr &obj )
        {
            return is_numeric( obj.get( ) );
        }

        static
        std::int64_t to_index( const objects::sptr &id )
        {
            std::int64_t index = std::numeric_limits<std::int64_t>::max( );

            if( id->get_type( ) == objects::type::INTEGER ) {
                auto iid = objects::cast_int( id.get( ) );
                index = iid->value( );
            } else if( id->get_type( ) == objects::type::FLOAT ) {
                auto iid = objects::cast_float( id.get( ) );
                index = static_cast<decltype(index)>(iid->value( ));
            }
            return index;
        }

        static
        objects::sptr eval_in_table( infix * /*inf*/,
                                     objects::sptr lft, objects::sptr rght,
                                     environment::sptr /*env*/  )
        {
            auto tbl = objects::cast_table( rght.get( ) );
            auto f = tbl->value( ).find( lft );
            return objects::boolean::make(f != tbl->value( ).end( ));
        }

        static
        objects::sptr eval_in_array( infix * /*inf*/,
                                     objects::sptr lft, objects::sptr rght,
                                     environment::sptr /*env*/  )
        {
            using OB = objects::boolean;
            auto arr = objects::cast_array( rght.get( ) );

            for( auto &a: arr->value( ) ) {
                auto &val(a->value( ));
                if( val->get_type( ) == objects::type::FLOAT ) {

                    if( auto o = objects::numeric::to_float(lft) ) {
                        if( val->equal( o.get( ) ) ) {
                            return OB::make( true );
                        };
                    }

                } else if( lft->get_type( ) == objects::type::FLOAT ) {

                    if( auto o = objects::numeric::to_float(val) ) {
                        if( val->equal( o.get( ) ) ) {
                            return OB::make( true );
                        };
                    }

                } else {
                    if( val->equal( lft.get( ) ) ) {
                        return OB::make( true );
                    }
                }
            }

            return objects::boolean::make(false);
        }

        static
        objects::sptr eval_in_ival( infix * /*inf*/,
                                    objects::sptr lft, objects::sptr rght,
                                    environment::sptr /*env*/  )
        {
            auto ivl = objects::cast_ival( rght.get( ) );
            switch (ivl->domain( )) {
            case objects::type::STRING:
                if( lft->get_type( ) == objects::type::STRING ) {
                    return objects::boolean::make(ivl->contains(lft.get( ) ) );
                }
                break;
            case objects::type::BOOLEAN:
                if(auto o = objects::numeric::to_bool(lft)) {
                    return objects::boolean::make(ivl->contains(o.get( ) ) );
                }
                break;
            case objects::type::INTEGER:
                if(auto o = objects::numeric::to_int(lft)) {
                    return objects::boolean::make(ivl->contains(o.get( ) ) );
                }
                break;
            case objects::type::FLOAT:
                if(auto o = objects::numeric::to_float(lft)) {
                    return objects::boolean::make(ivl->contains(o.get( ) ) );
                }
                break;
            }
            return objects::boolean::make(false);;
        }

        static
        objects::sptr common_infix( infix *inf,
                                    objects::sptr left, objects::sptr right,
                                    environment::sptr env )

        {

            switch (right->get_type( )) {
            case objects::type::BUILTIN:
                if( inf->token( ) == tokens::type::BIT_OR ) {
                    return eval_builtin( inf, left, right, env);
                }
                break;
            case objects::type::FUNCTION:
                if( inf->token( ) == tokens::type::BIT_OR ) {
                    return eval_func( inf, left, right, env);
                }
                break;
            case objects::type::TABLE:
                if( inf->token( ) == tokens::type::IN ) {
                    return eval_in_table( inf, left, right, env );
                }
                break;
            case objects::type::INTERVAL:
                if( inf->token( ) == tokens::type::IN ) {
                    return eval_in_ival( inf, left, right, env );
                }
                break;
            case objects::type::ARRAY:
                if( inf->token( ) == tokens::type::IN ) {
                    return eval_in_array( inf, left, right, env );
                }
                break;
            default:
                break;
            }

            return error_type::make(inf->pos( ), "Infix operation ",
                                    left->get_type( )," '",
                                    inf->token( ), "' ",
                                    right->get_type( ),
                                    " is not defined");
        }

        static
        objects::sptr eval_builtin( infix * /*inf*/,
                                    objects::sptr obj, objects::sptr call,
                                    environment::sptr /*env*/ )
        {
            objects::slist par { obj };
            auto func     = objects::cast_builtin(call.get( ));
            auto call_env = environment::make( func->env( ) );
            return objects::tail_call::make( call, std::move(par), call_env );
        }

        static
        objects::sptr eval_equal( infix *inf,
                                  objects::sptr lft, objects::sptr rght )
        {
            if( lft->get_type( ) == rght->get_type( ) ) {
                bool res = lft->equal( rght.get( ) );
                if( inf->token( ) == tokens::type::NOT_EQ ) {
                    res = !res;
                }
                return objects::boolean::make( res );
            }
            return error_type::make( inf->pos( ), "Operation ",
                                     lft->get_type( )," '",
                                     inf->token( ), "' ",
                                     rght->get_type( ),
                                     " is not defined");
        }

        static
        objects::sptr eval_func( infix *inf,
                                 objects::sptr obj, objects::sptr call,
                                 environment::sptr /*env*/ )
        {
            auto func = objects::cast_func(call.get( ));
            auto call_env = environment::make( func->env( ) );

            if( func->param_size( ) == 0 ) {
                return error_type::make( inf->pos( ),
                                  "Function does not accept parameters");
            }

            if( func->param_size( ) > 1 ) {
                auto new_env = environment::make(func->env( ));
                auto &p( *func->begin( ) );

                if( is_ident( p ) ) {
                    auto n = static_cast<ast::expressions::ident *>(p.get( ));
                    new_env->set( n->value( ), obj );
                } else {
                    return error_type::make( inf->pos( ),
                                             "Invalid argument ", 1,
                                              p->str( ) );
                }
                return objects::function::make( new_env, *func, 1 );
            }

            for( auto &p: *func ) {
                if( !is_ident( p ) ) {
                    return error_type::make( inf->pos( ),
                                             "Invalid parameter type.",
                                             p->get_type( ));
                }
                call_env->set( p->str( ), obj );
            }
            return objects::tail_call::make( call, call_env );
        }

    };


}}}

#endif // COMMON_OPERATIONS_H
