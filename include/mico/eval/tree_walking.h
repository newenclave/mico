#ifndef MICO_TREE_WALKING_H
#define MICO_TREE_WALKING_H

#include <iostream>

#include "mico/eval/evaluator.h"
#include "mico/expressions/expressions.h"
#include "mico/statements.h"
#include "mico/tokens.h"
#include "mico/environment.h"
#include "mico/eval/operations/integer.h"
#include "mico/eval/operations/boolean.h"
#include "mico/eval/operations/float.h"
#include "mico/eval/operations/string.h"
#include "mico/eval/operations/tables.h"
#include "mico/eval/operations/arrays.h"
#include "mico/eval/operations/function.h"

namespace mico { namespace eval {

    class tree_walking: public base {

    public:
        using error_list = std::vector<std::string>;
    private:

        template <objects::type T>
        using OP = operations::operation<T>;

        ////////////// errors /////////////

        objects::sptr error_operation_notfound( tokens::type tt,
                                                const ast::node *n )
        {
            return objects::error::make( n->pos( ),
                                         "operation '", tt, "' not found" );
        }

        objects::sptr error_index_inval( const ast::node *n,
                                         objects::sptr val )
        {
            return objects::error::make( n->pos( ),
                                         "invalid index for '", val, "'" );
        }

        template <typename ...Args>
        objects::sptr error( const ast::node *n, Args&&...args )
        {
            return objects::error::make( n, std::forward<Args>(args)... );
        }

        //////////////////////////////////
        static
        objects::sptr get_null( )
        {
            static auto nobj = objects::null::make( );
            return nobj;
        }

        template <typename ToT, typename FromT>
        ToT *obj_cast( FromT *obj )
        {
#ifdef DEBUG
//            if(objects::<ToT>::type_name == obj->get_type( )) {
//                throw std::runtime_error("Invalid type conversion;");
//            }
#endif
            return static_cast<ToT *>(obj);
        }

        static
        bool is_null( const objects::sptr &obj )
        {
            return (!obj) || (obj->get_type( ) == objects::type::NULL_OBJ);
        }

        static
        bool is_fail( const objects::sptr &obj )
        {
            return (!!obj) && (obj->get_type( ) == objects::type::FAILURE);
        }

        static
        bool is_return( const objects::sptr &obj )
        {
            return ((!!obj) && (obj->get_type( ) == objects::type::RETURN));
        }

        static
        bool is_func( objects::sptr obj )
        {
            return  (!!obj) &&
                 (  (obj->get_type( ) == objects::type::FUNCTION)
                 || (obj->get_type( ) == objects::type::BUILTIN) );
        }

        static
        bool is_numeric( objects::sptr obj )
        {
            return (!!obj) &&
                    ( (obj->get_type( ) == objects::type::INTEGER)
                   || (obj->get_type( ) == objects::type::FLOAT) );
        }

        static
        bool is_fail_args( const objects::slist &args )
        {
            return ( args.size( ) == 1 ) && ( is_fail( args[0] ) );
        }

        static
        bool is_function_call( const ast::expression *exp )
        {
            return (exp->get_type( ) == ast::type::CALL);
//            if(exp->get_type( ) == ast::type::CALL) {
//                return true;
//                auto call = static_cast<const ast::expressions::call *>(exp);
//                return call->func( )->get_type( ) == ast::type::FN;
//            }
//            return false;
        }

        static
        objects::retutn_obj::sptr do_return( objects::sptr res )
        {
            return objects::retutn_obj::make( res );
        }

        static
        objects::boolean::sptr get_bool_true( )
        {
            static auto bobj = objects::boolean::make(true);
            return bobj;
        }


        objects::boolean::sptr get_bool( bool b )
        {
            return b ? get_bool_true( ) : get_bool_false( );
        }

        static
        objects::boolean::sptr get_bool_false( )
        {
            static auto bobj = objects::boolean::make(false);
            return bobj;
        }

        objects::boolean::sptr get_bool( ast::node *n )
        {
            auto bstate = static_cast<ast::expressions::boolean *>(n);
            return bstate->value( ) ? get_bool_true( ) : get_bool_false( );
        }


        objects::integer::sptr eval_int( ast::node *n )
        {
            auto state = static_cast<ast::expressions::integer *>(n);
            return std::make_shared<objects::integer>( state->value( ) );
        }

        objects::integer::sptr get_signed( ast::node *n )
        {
            auto state = static_cast<ast::expressions::integer *>(n);
            auto sign_value = static_cast<std::int64_t>(state->value( ));
            return std::make_shared<objects::integer>( sign_value );
        }

        objects::integer::sptr get_signed( std::int64_t n )
        {
            return std::make_shared<objects::integer>( n );
        }

        static
        environment::sptr object_env( objects::sptr o )
        {
            switch (o->get_type( )) {
            case objects::type::FUNCTION:
                return objects::cast_func(o.get( ))->env( );
            case objects::type::BUILTIN:
                return objects::cast_builtin(o.get( ))->env( );
            case objects::type::TAIL_CALL:
                return objects::cast_tail_call(o.get( ))->env( );
            }
            return environment::sptr( );
        }

        objects::sptr extract_return( objects::sptr obj )
        {
            if( obj->get_type( ) == objects::type::RETURN ) {
                auto ret = obj_cast<objects::retutn_obj>(obj.get( ));
                return ret->value( );
            } else {
                return obj;
            }
        }

        objects::sptr unref( objects::sptr obj )
        {
            return objects::reference::unref( obj );
        }

        objects::sptr eval_float( ast::node *n )
        {
            auto state = static_cast<ast::expressions::floating *>(n);
            return std::make_shared<objects::floating>( state->value( ) );
        }

        objects::floating::sptr eval_float( double n )
        {
            return std::make_shared<objects::floating>( n );
        }

        objects::string::sptr eval_string( ast::node *n )
        {
            auto val = static_cast<ast::expressions::string *>(n);
            return std::make_shared<objects::string>( val->value( ) );
        }

        objects::sptr produce_prefix_minus( ast::expressions::prefix *n,
                                            environment::sptr env )
        {
            auto oper = unref(eval_impl(n->value( ), env));
            switch ( oper->get_type( ) ) {
            case objects::type::INTEGER: {
                auto o = obj_cast<objects::integer>(oper.get( ));
                return get_signed( -1 * static_cast<std::int64_t>(o->value( )));
            }
            case objects::type::FLOAT: {
                auto o = obj_cast<objects::floating>(oper.get( ));
                return eval_float( -1.0 * o->value( ) );
            }
            default:
                break;
            }
            return get_null( );
        }

        bool is_bool( objects::base *oper )
        {
            return (oper->get_type( ) == objects::type::INTEGER )
                || (oper->get_type( ) == objects::type::FLOAT )
                || (oper->get_type( ) == objects::type::BOOLEAN )
                || (oper->get_type( ) == objects::type::NULL_OBJ )
                 ;
        }

        objects::sptr eval_prefix( ast::node *n, environment::sptr env )
        {
            auto expr = static_cast<ast::expressions::prefix *>( n );
            auto oper = eval_impl(expr->value( ), env);

            if( is_fail(oper) ) {
                return oper;
            }

            using int_operation   = OP<objects::type::INTEGER>;
            using float_operation = OP<objects::type::FLOAT>;
            using bool_operation  = OP<objects::type::BOOLEAN>;
            using str_operation   = OP<objects::type::STRING>;
            using table_operation = OP<objects::type::TABLE>;
            using array_operation = OP<objects::type::ARRAY>;
            using func_operation  = OP<objects::type::FUNCTION>;

            objects::type opertype = oper->get_type( );
            if( opertype == objects::type::REFERENCE ) {
                opertype = objects::cast_ref( oper )->value( )->get_type( );
            }

            switch ( opertype ) {
            case objects::type::INTEGER:
                return int_operation::eval_prefix(expr, oper);
            case objects::type::STRING:
                return str_operation::eval_prefix(expr, oper);
            case objects::type::BOOLEAN:
                return bool_operation::eval_prefix(expr, oper);
            case objects::type::FLOAT:
                return float_operation::eval_prefix(expr, oper);
            case objects::type::TABLE:
                return table_operation::eval_prefix(expr, oper);
            case objects::type::ARRAY:
                return array_operation::eval_prefix(expr, oper);
            case objects::type::BUILTIN:
            case objects::type::FUNCTION:
                return func_operation::eval_prefix(expr, oper);
            default:
                break;
            }

            return error( n, "Invalid prefix function '",
                          expr->token( ), "'" );
        }

        template <typename Res>
        Res obj2num( objects::base *oper )
        {
            switch (oper->get_type( )) {
            case objects::type::BOOLEAN: {
                auto val = objects::cast_bool(oper)->value( ) ? 1 : 0;
                return static_cast<Res>( val );
            }
            case objects::type::INTEGER: {
                auto val = objects::cast_int(oper)->value( );
                return static_cast<Res>( val );
            }
            case objects::type::FLOAT: {
                auto val = objects::cast_float(oper)->value( );
                return static_cast<Res>( val );
            }
            default:
                break;
            }
            return Res( );
        }

        template <typename NumObj>
        objects::sptr obj2num_obj( objects::base *oper )
        {
            switch (oper->get_type( )) {
            case objects::type::BOOLEAN:
                return get_bool( obj2num<bool>(oper) );
            case objects::type::INTEGER:
                return std::make_shared<NumObj>( obj2num<std::int64_t>(oper) );
            case objects::type::FLOAT:
                return std::make_shared<NumObj>( obj2num<double>(oper) );
            default:
                break;
            }
            return get_null( );
        }

        environment::sptr make_env( environment::sptr parent )
        {
            return environment::make(parent);
        }

        objects::sptr eval_assign( ast::expressions::infix *inf,
                                   environment::sptr env )
        {

            if(inf->left( )->get_type( ) == ast::type::IDENT) {
                using ident_type = ast::expressions::ident;
                auto id = static_cast<ident_type *>(inf->left( ).get( ));

                auto cont_env = env->find_contains( id->value( ) );
                if( !cont_env ) {
                    return error( id, "Identifier not found: '",
                                  id->value( ), "'" );
                }

                auto rght = eval_impl_tail(inf->right( ).get( ), env);
                if( is_fail( rght ) ) {
                    return rght;
                }

                cont_env->set( id->value( ), unref(rght) );
                return rght;
            } else {
                auto lft = eval_impl_tail( inf->left( ).get( ), env );
                if( lft->get_type( ) == objects::type::REFERENCE ) {
                    auto cont = obj_cast<objects::reference>(lft.get( ));
                    auto rght = eval_impl_tail(inf->right( ).get( ), env);
                    if( is_fail( rght ) ) {
                        return rght;
                    }
                    cont->set_value(env.get( ), rght->clone( ));
                    return cont->value( );
                }
            }
            return error( inf, "Invalid left value for ASSIGN ",
                          inf->left( ) );
        }

        objects::sptr eval_equal( objects::base *lft, objects::base *rgh,
                                  ast::expressions::infix *inf,
                                  environment::sptr /*env*/ )
        {
            auto copmarable =
                    (
                        (lft->get_type( ) == rgh->get_type( ))
                    &&  (lft->get_type( ) != objects::type::FUNCTION)
                    &&  (lft->get_type( ) != objects::type::TAIL_CALL)
                    );

            if( !copmarable ) {
                return objects::error::make( inf->pos( ),
                                         "Impossible to compare ",
                                         inf->left( )->get_type( ),
                                             " and ",
                                         inf->right( )->get_type( ) );
            }
            bool eq = lft->equal( rgh );
            return get_bool((inf->token( ) == tokens::type::EQ) ? eq : !eq);
        }

        objects::sptr eval_infix( ast::node *n, environment::sptr env )
        {
            auto inf = static_cast<ast::expressions::infix *>(n);

            /// need to do some optimization for OR, AND operation

            if( inf->token( ) == tokens::type::ASSIGN ) {
                return eval_assign( inf, env );
            }

            auto left = unref(eval_impl_tail(inf->left( ).get( ), env));
            if( !left ) {
                /////////// bad left value
                return get_null( );
            }

            auto inf_call = [this, env](ast::node *n ) {
                return unref( eval_impl( n, env ) );
            };

            using int_operation   = OP<objects::type::INTEGER>;
            using float_operation = OP<objects::type::FLOAT>;
            using bool_operation  = OP<objects::type::BOOLEAN>;
            using str_operation   = OP<objects::type::STRING>;
            using table_operation = OP<objects::type::TABLE>;
            using array_operation = OP<objects::type::ARRAY>;
            using func_operation  = OP<objects::type::FUNCTION>;

            switch (left->get_type( )) {
            case objects::type::INTEGER:
                return int_operation::eval_infix( inf, left, inf_call, env);
            case objects::type::FLOAT:
                return float_operation::eval_infix( inf, left, inf_call, env);
            case objects::type::BOOLEAN:
                return bool_operation::eval_infix( inf, left, inf_call, env);
            case objects::type::STRING:
                return str_operation::eval_infix( inf, left, inf_call, env);
            case objects::type::TABLE:
                return table_operation::eval_infix( inf, left, inf_call, env);
            case objects::type::ARRAY:
                return array_operation::eval_infix( inf, left, inf_call, env);
            case objects::type::FUNCTION:
            case objects::type::BUILTIN:
                return func_operation::eval_infix( inf, left, inf_call, env);
            default:
                break;
            }

            return error_operation_notfound( inf->token( ), inf );
        }

        environment::sptr create_call_env( ast::expressions::call *call,
                                           objects::base *fun,
                                           environment::sptr env,
                                           objects::slist &params )
        {
            //// TODO bug with built in funcions!

            using ident_type = ast::expressions::ident;

            if( fun->get_type( ) == objects::type::FUNCTION ) {
                auto vfun = obj_cast<objects::function>(fun);

                auto new_env = environment::make(vfun->env( ));

                if( call->params( ).size( ) != vfun->params( ).size( ) ) {
                    //// TODO bad params count
                    return nullptr;
                }
                size_t id = 0;
                for( auto &p: vfun->params( ) ) {
                    if( p->get_type( ) == ast::type::IDENT ) {
                        auto n = static_cast<ident_type *>(p.get( ));
                        auto v = unref(
                                 eval_impl_tail( call->params( )[id++].get( ),
                                                 env ) );
                        new_env->set(n->value( ), v);
                    } else {
                        /// TODO bad param
                        return nullptr;
                    }
                }
                return new_env;
            } else if( fun->get_type( ) == objects::type::BUILTIN ) {

                auto vfun = obj_cast<objects::builtin>(fun);
                auto new_env = environment::make(vfun->env( ));

                for( auto &cp: call->params( ) ) {
                    auto v = unref( eval_impl_tail( cp.get( ), env ) );
                    params.push_back( v );
                }
                return new_env;
            }
        }

        objects::sptr get_cont_call( ast::node *n, environment::sptr env )
        {
            auto call = static_cast<ast::expressions::call *>( n );
            auto fun = eval_impl(call->func( ), env);
            if( is_null( fun ) || !is_func( fun ) ) {
                ///// TODO error call object
                return error(n, "It is not a callable object");
            }

            objects::slist params;
            auto new_env = create_call_env( call, fun.get( ), env, params );
            if( !new_env ) {
                return get_null( );
            }
            return std::make_shared<objects::tail_call>(fun, params, new_env);
        }

        objects::sptr eval_tail( objects::sptr obj )
        {
            using call_type = objects::tail_call;
            while(obj->get_type( ) == objects::type::TAIL_CALL ) {
                auto call = static_cast<call_type *>(obj.get( ));
                auto call_type = call->value( )->get_type( );
                if( call_type == objects::type::FUNCTION ) {
                    auto fun = obj_cast<objects::function>
                                        (call->value( ).get( ));
                    environment::scoped s( call->env( ) );
                    fun->env( )->GC( );
                    obj = eval_scope_impl( fun->body( ), call->env( ) );
                } else if( call_type == objects::type::BUILTIN ) {
                    auto fun = obj_cast<objects::builtin>
                                        (call->value( ).get( ));
                    obj = fun->call( call->params( ), call->env( ) );

                }
                if( is_return( obj ) ) {
                    return obj;
                }
            }
            return obj;
        }

        objects::sptr eval_scope( ast::statement_list &lst,
                                  environment::sptr env )
        {
            objects::sptr res = eval_scope_impl(lst, env);
            return eval_tail( res );
        }

        objects::sptr eval_scope_impl( ast::statement_list &lst,
                                       environment::sptr env )
        {
            using return_type = ast::statements::ret;
            using expression_type = ast::statements::expr;
            auto count = lst.size( );
            objects::sptr last = get_null( );
            for( auto &stmt: lst ) {

                --count;

                if(stmt->get_type( ) == ast::type::RETURN) {
                    auto expr = static_cast<return_type *>( stmt.get( ) );
                    if( is_function_call( expr->value( ) ) ) {
                        return do_return(get_cont_call( expr->value( ), env ) );
                    }
                }

                if( 0 == count ) {
                    if(stmt->get_type( ) == ast::type::EXPR) {
                        auto expr = static_cast<expression_type *>(stmt.get( ));
                        if( is_function_call( expr->value( ).get( ) ) ) {
                            return get_cont_call(expr->value( ).get( ), env);
                        }
                    }
                }

                auto next = eval_impl( stmt.get( ), env );
                last = next;
                //last = eval_tail(next);
                if( is_return( next ) || is_fail( next ) ) {
                    return last;
                }
            }
            return last;
        }

        objects::sptr eval_ifelse( ast::node *n, environment::sptr env )
        {
            auto ifblock = static_cast<ast::expressions::ifelse *>( n );

            for( auto &i: ifblock->ifs(  ) ) {
                auto cond = unref(eval_impl( i.cond.get( ), env ));
                if( is_null( cond ) ) {
                    /////////// TODO bad condition in if
                    return get_null( );
                }
                auto res = obj2num_obj<objects::boolean>( cond.get( ) );
                if( is_null( res ) ) {
                    /////////// TODO bad result type for cond
                    return get_null( );
                }
                auto bres = obj_cast<objects::boolean>(res.get( ));
                if( bres->value( ) ) {
                    environment::scoped s(make_env(env));
                    auto eval_states = eval_scope_impl( i.states, s.env( ));
                    return unref(eval_states);
                }
            }
            if( !ifblock->alt( ).empty( ) ) {
                environment::scoped s(make_env(env));
                auto eval_states = eval_scope_impl( ifblock->alt( ), s.env( ) );
                return unref(eval_states);
            }
            return get_null( );
        }

        objects::sptr eval_program( ast::node *n, environment::sptr env )
        {
            auto prog = static_cast<ast::program *>( n );
            objects::sptr last = get_null( );
            for( auto &s: prog->states( ) ) {
                last = eval_impl_tail( s.get( ), env );
                if( is_return(last) ) {
                    return eval_tail(extract_return( last ));
                }
            }
            return unref(last);
        }

        objects::sptr eval_expression( ast::node *n, environment::sptr env )
        {
            auto expr = static_cast<ast::statements::expr *>( n );
            return eval_impl( expr->value( ).get( ), env );
        }

        objects::sptr eval_let( ast::node *n, environment::sptr env )
        {
            auto expr = static_cast<ast::statements::let *>( n );
            auto id   = expr->ident( )->str( );
            auto val  = eval_impl( expr->value( ), env );
            if( is_fail( val ) ) {
                return val;
            }
            env->set( id, unref(val) );
            return get_null( );
        }

        objects::sptr eval_return( ast::node *n, environment::sptr env )
        {
            auto expr = static_cast<ast::statements::ret *>( n );
            auto val  = eval_impl( expr->value( ), env );
            return std::make_shared<objects::retutn_obj>(val);
        }

        objects::sptr eval_ident( ast::node *n, environment::sptr env )
        {

            auto expr = static_cast<ast::expressions::ident *>( n );
            auto val = env->get( expr->value( ) );
            if( !val ) {
                return error( n, "Identifier not found '", n->str( ), "'" );
            } else {
                return val;
            }
        }

        objects::sptr eval_function( ast::node *n, environment::sptr env )
        {
            auto func = static_cast<ast::expressions::function *>( n );
            auto fff  = std::make_shared<objects::function>( make_env(env),
                                              func->ident_ptr( ),
                                              func->expr_ptr( ));
            //env->add_keep(fff.get( ), fff);
            return fff;
        }

        objects::sptr eval_index( ast::node *n, environment::sptr env )
        {
            auto idx = static_cast<ast::expressions::index *>(n);

            auto val = unref(eval_impl_tail( idx->value( ), env ));

            if( is_fail( val ) ) {
                return val;
            }

            if( val->get_type( ) == objects::type::STRING ) {

                auto str = obj_cast<objects::string>( val.get( ) );
                auto id = eval_impl_tail( idx->param( ), env );
                std::size_t index = str->value( ).size( );

                if( id->get_type( ) == objects::type::INTEGER ) {
                    auto iid = obj_cast<objects::integer>( id.get( ) );
                    index = static_cast<std::size_t>(iid->value( ));
                } else if( id->get_type( ) == objects::type::FLOAT ) {
                    auto iid = obj_cast<objects::floating>( id.get( ) );
                    index = static_cast<std::size_t>(iid->value( ));
                } else {
                    return error( idx->param( ), idx->param( ),
                                  " has invalid type; must be integer" );
                }
                if( index < str->value( ).size( ) ) {
                    return objects::integer::make( str->value( )[index] );
                } else {
                    return error( idx->param( ), idx->param( ),
                                  " is not a valid index for the string" );
                }

            } else if(val->get_type( ) == objects::type::ARRAY) {

                auto arr = obj_cast<objects::array>( val.get( ) );
                auto id = eval_impl_tail( idx->param( ), env );
                std::int64_t index = arr->value( ).size( );

                if( id->get_type( ) == objects::type::INTEGER ) {
                    auto iid = obj_cast<objects::integer>( id.get( ) );
                    index = static_cast<decltype(index)>(iid->value( ));
                } else if( id->get_type( ) == objects::type::FLOAT ) {
                    auto iid = obj_cast<objects::floating>( id.get( ) );
                    index = static_cast<decltype(index)>(iid->value( ));
                } else {
                    return error( idx->param( ), idx->param( ),
                                  " has invalid type; must be integer" );
                }
                if( static_cast<std::size_t>(index) < arr->value( ).size( ) ) {
                    return arr->at( index );
                } else {
                    return error( idx->param( ), idx->param( ),
                                  " is not a valid index for the array" );
                }

            } else if(val->get_type( ) == objects::type::TABLE) {

                auto arr = obj_cast<objects::table>( val.get( ) );
                auto id = unref(eval_impl( idx->param( ), env ));

                if( is_fail( id ) ) {
                    return id;
                }

                auto res = arr->at( id );
                if( !is_null( res ) ) {
                    return res;
                } else {
                    return error( idx->param( ), idx->param( ),
                                  " is not a valid index for the table" );
                }

            } else {

            }

            return objects::error::make(n->pos( ),
                                        "Impossible to get an index of ",
                                        idx->param( )->get_type( ));
        }

        objects::slist eval_parameters( ast::expressions::call *call,
                                        environment::sptr env )
        {
            objects::slist res;
            for( auto &e: call->params( ) ) {
                auto next = unref(eval_impl_tail( e.get( ), env ));
                if( is_fail(next) ) {
                    return objects::slist { next };
                }
                res.emplace_back( next );
            }
            return res;
        }

        objects::sptr eval_call_impl( ast::node *n,
                                      environment::sptr env,
                                      environment::sptr &/*work_env*/ )
        {
            auto call = static_cast<ast::expressions::call *>( n );
            auto fun = unref(eval_impl(call->func( ), env));
            if( is_fail( fun ) ) {
                return fun;
            }
            if( is_null( fun ) || !is_func( fun ) ) {
                return error( call->func( ),
                              fun->get_type( ), "(", fun, ")",
                              " is not a callable object" );
            }

            if( fun->get_type( ) == objects::type::FUNCTION ) {

                auto vfun = obj_cast<objects::function>(fun.get( ));

                vfun->env( )->GC( );

                objects::slist params;

                environment::scoped s(create_call_env( call, vfun,
                                                       env, params ) );

                if( !s.env( ) ) {
                    return get_null( );
                }

                auto res = eval_scope( vfun->body( ), s.env( ) );
                while( is_return( res ) ) {
                    auto r = obj_cast<objects::retutn_obj>(res.get( ));
                    res = eval_tail( r->value( ) );
                }

                return res;

            } else if( fun->get_type( ) == objects::type::BUILTIN ) {
                environment::scoped s(make_env( env ));
                auto vfun = obj_cast<objects::builtin>(fun.get( ));
                vfun->init( s.env( ) );
                auto params = eval_parameters(call, s.env( ));
                if( params.size( ) == 1 && is_fail( params[0] ) ) {
                    return params[0];
                }
                auto res = vfun->call( params, s.env( ) );
                return res;
            }
            return error( call->func( ), "Internal error: ",
                          fun, " is function, but it is not in the call list" );
        }

        objects::sptr eval_call( ast::node *n, environment::sptr env )
        {
            environment::sptr we;
            auto res = eval_call_impl( n, env, we );
            return res;
        }

        objects::sptr eval_array( ast::node *n, environment::sptr env )
        {
            auto arr = static_cast<ast::expressions::array *>( n );
            auto res = objects::array::make( env );

            for( auto &a: arr->value( ) ) {
                auto next = eval_impl( a.get( ), env );
                if( is_fail( next ) ) {
                    return next;
                }
                res->push( env.get( ), unref(next) );
            }

            return res;
        }

        objects::sptr eval_table( ast::node *n, environment::sptr env )
        {
            auto table = static_cast<ast::expressions::table *>( n );

            auto res = objects::table::make( env );

            for( auto &v: table->value( ) ) {
                auto key = unref( eval_impl_tail( v.first.get( ), env ) );
                if( is_fail( key ) ) {
                    return key;
                }

                if( (key->get_type( ) == objects::type::FUNCTION) ||
                    (key->get_type( ) == objects::type::BUILTIN ) ) {
                    return error( v.first.get( ), "unusable as hash key: ",
                                                   key->get_type( ));
                }

                auto val = unref(eval_impl_tail( v.second.get( ), env ));
                if( is_fail( val ) ) {
                    return val;
                }
                res->set( env.get( ), key, val );
            }

            return res;
        }

        objects::sptr eval_impl_tail( ast::node *n, environment::sptr env )
        {
            auto res = eval_impl(n, env);
            return eval_tail( res );
        }

        objects::sptr eval_impl( ast::node *n, environment::sptr env )
        {
            objects::sptr res = get_null( );
            switch (n->get_type( )) {
            case ast::type::PROGRAM:
                res = eval_program( n, env ); break;
            case ast::type::EXPR:
                res = eval_expression( n, env ); break;
            case ast::type::BOOLEAN:
                res = get_bool( n ); break;
            case ast::type::INTEGER:
                res = eval_int( n ); break;
            case ast::type::FLOAT:
                res = eval_float( n ); break;
            case ast::type::STRING:
                res = eval_string( n ); break;
            case ast::type::ARRAY:
                res = eval_array( n, env ); break;
            case ast::type::PREFIX:
                res = eval_prefix( n, env ); break;
            case ast::type::INFIX:
                res = eval_infix( n, env ); break;
            case ast::type::IFELSE:
                res = eval_ifelse( n, env ); break;
            case ast::type::INDEX:
                res = eval_index( n, env ); break;
            case ast::type::IDENT:
                res = eval_ident( n, env ); break;
            case ast::type::LET:
                res = eval_let( n, env ); break;
            case ast::type::RETURN:
                res = eval_return( n, env ); break;
            case ast::type::FN:
                res = eval_function( n, env ); break;
            case ast::type::CALL:
                res = eval_call( n, env ); break;
            case ast::type::TABLE:
                res = eval_table( n, env ); break;
            case ast::type::NONE:
                break;
            }
            return res;
        }


    public:

        objects::sptr eval( ast::node *n, environment::sptr env ) override
        {
            return eval_impl( n, env );
        }

    private:
        error_list errors_;
    };

}}

#endif // TREE_WALKING_H
