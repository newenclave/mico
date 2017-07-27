#ifndef MICO_TREE_WALKING_H
#define MICO_TREE_WALKING_H

#include <iostream>
#include <limits>

#include "mico/eval/evaluator.h"
#include "mico/expressions.h"
#include "mico/statements.h"
#include "mico/tokens.h"
#include "mico/state.h"
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

        tree_walking( )
        { }

        ~tree_walking( )
        { }

    private:

        template <objects::type T>
        using OP = operations::operation<T>;

        ////////////// errors /////////////

        static
        objects::sptr error_operation_notfound( tokens::type tt,
                                                const ast::node *n )
        {
            return objects::error::make( n->pos( ),
                                         "operation '", tt, "' not found" );
        }

        template <typename ...Args>
        static
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

        template <objects::type ToT, typename FromT>
        objects::impl<ToT> *obj_cast( FromT *obj )
        {
            return objects::cast<ToT>(obj);
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
        bool is_function_call( const ast::node *exp )
        {
            return (exp->get_type( ) == ast::type::CALL);
        }

        static
        bool is_tail_call( const objects::base *exp )
        {
            return (exp->get_type( ) == objects::type::TAIL_CALL );
        }

        static
        objects::retutn_obj::sptr do_return( objects::sptr res )
        {
            return objects::retutn_obj::make( res );
        }

        static
        objects::boolean::sptr get_bool_value( bool b )
        {
            return objects::boolean::make( b );
        }

        static
        objects::sptr unref( objects::sptr obj )
        {
            return objects::reference::unref( obj );
        }

        static
        environment::sptr make_env( environment::sptr parent )
        {
            return environment::make(parent);
        }

        objects::boolean::sptr get_bool( const ast::node *n )
        {
            auto bstate = static_cast<const ast::expressions::boolean *>(n);
            return get_bool_value(bstate->value( ));
        }

        objects::integer::sptr eval_int( ast::node *n )
        {
            auto state = ast::cast<ast::expressions::integer>(n);
            return std::make_shared<objects::integer>( state->value( ) );
        }

        objects::sptr extract_return( objects::sptr obj )
        {
            if( obj->get_type( ) == objects::type::RETURN ) {
                auto ret = obj_cast<objects::type::RETURN>(obj.get( ));
                return ret->value( );
            } else {
                return obj;
            }
        }

        objects::sptr eval_float( ast::node *n )
        {
            auto state = ast::cast<ast::expressions::floating>(n);
            return std::make_shared<objects::floating>( state->value( ) );
        }

        objects::string::sptr eval_string( ast::node *n )
        {
            auto val = ast::cast<ast::expressions::string>(n);
            return std::make_shared<objects::string>( val->value( ) );
        }

        objects::sptr eval_prefix( ast::node *n, environment::sptr env )
        {
            auto expr = ast::cast<ast::expressions::prefix>( n );
            auto oper = eval_impl(expr->value( ).get( ), env);

            if( is_fail(oper) ) {
                return oper;
            }

            using OP_int   = OP<objects::type::INTEGER>;
            using OP_float = OP<objects::type::FLOAT>;
            using OP_bool  = OP<objects::type::BOOLEAN>;
            using OP_str   = OP<objects::type::STRING>;
            using OP_table = OP<objects::type::TABLE>;
            using OP_array = OP<objects::type::ARRAY>;
            using OP_func  = OP<objects::type::FUNCTION>;

            objects::type opertype = oper->get_type( );
            if( opertype == objects::type::REFERENCE ) {
                opertype = objects::cast_ref( oper )->value( )->get_type( );
            }

            switch ( opertype ) {
            case objects::type::INTEGER:
                return OP_int::eval_prefix(expr, oper);
            case objects::type::STRING:
                return OP_str::eval_prefix(expr, oper);
            case objects::type::BOOLEAN:
                return OP_bool::eval_prefix(expr, oper);
            case objects::type::FLOAT:
                return OP_float::eval_prefix(expr, oper);
            case objects::type::TABLE:
                return OP_table::eval_prefix(expr, oper);
            case objects::type::ARRAY:
                return OP_array::eval_prefix(expr, oper);
            case objects::type::BUILTIN:
            case objects::type::FUNCTION:
                return OP_func::eval_prefix(expr, oper);
            default:
                break;
            }

            return error( n, "Invalid prefix function '",
                          expr->token( ), "' for ",
                          expr->value( )->get_type( ) );
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
                return get_bool_value( obj2num<bool>(oper) );
            case objects::type::INTEGER:
                return std::make_shared<NumObj>( obj2num<std::int64_t>(oper) );
            case objects::type::FLOAT:
                return std::make_shared<NumObj>( obj2num<double>(oper) );
            default:
                break;
            }
            return get_null( );
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

                auto rght = unref( eval_impl_tail(inf->right( ).get( ), env) );
                if( is_fail( rght ) ) {
                    return rght;
                }

                cont_env->set( id->value( ), unref(rght) );
                return rght;
            } else {
                auto lft = eval_impl_tail( inf->left( ).get( ), env );
                if( lft->get_type( ) == objects::type::REFERENCE ) {
                    auto cont = objects::cast_ref(lft.get( ));
                    auto rght = unref(eval_impl_tail(inf->right( ).get( ),
                                                     env ) );
                    if( is_fail( rght ) ) {
                        return rght;
                    }
                    cont->set_value(env.get( ), rght->clone( ));
                    return cont->value( );
                }
            }
            return error( inf, "Invalid left value for ASSIGN ",
                          inf->left( ).get( ) );
        }

        objects::sptr eval_infix( ast::node *n, environment::sptr env )
        {
            auto inf = ast::cast<ast::expressions::infix>(n);

            /// need to do some optimization for OR, AND operation

            if( inf->token( ) == tokens::type::ASSIGN ) {
                return eval_assign( inf, env );
            }

            auto left = unref(eval_impl_tail(inf->left( ).get( ), env));
            if( is_fail(left) ) {
                return left;
            }

            objects::type opertype = left->get_type( );
            if( opertype == objects::type::REFERENCE ) {
                opertype = objects::cast_ref( left )->value( )->get_type( );
            }

            auto inf_call = [this, env](ast::node *n ) {
                return unref( eval_impl_tail( n, env ) );
            };

            using OP_int   = OP<objects::type::INTEGER>;
            using OP_float = OP<objects::type::FLOAT>;
            using OP_bool  = OP<objects::type::BOOLEAN>;
            using OP_str   = OP<objects::type::STRING>;
            using OP_table = OP<objects::type::TABLE>;
            using OP_array = OP<objects::type::ARRAY>;
            using OP_func  = OP<objects::type::FUNCTION>;

            objects::sptr res;
            switch( opertype ) {
            case objects::type::INTEGER:
                res = OP_int::eval_infix( inf, left, inf_call, env);
                break;
            case objects::type::FLOAT:
                res = OP_float::eval_infix( inf, left, inf_call, env);
                break;
            case objects::type::BOOLEAN:
                res = OP_bool::eval_infix( inf, left, inf_call, env);
                break;
            case objects::type::STRING:
                res = OP_str::eval_infix( inf, left, inf_call, env);
                break;
            case objects::type::TABLE:
                res = OP_table::eval_infix( inf, left, inf_call, env);
                break;
            case objects::type::ARRAY:
                res = OP_array::eval_infix( inf, left, inf_call, env);
                break;
            case objects::type::FUNCTION:
            case objects::type::BUILTIN:
                res = OP_func::eval_infix( inf, left, inf_call, env);
                break;
            default:
                break;
            }
            if( res ) {
                return eval_tail( res );
            }

            return error_operation_notfound( inf->token( ), inf );
        }

        environment::sptr create_call_env( ast::expressions::call *call,
                                           objects::base *fun,
                                           environment::sptr env,
                                           objects::slist &params )
        {
            //// TODO bug with built in funcions!

            using elipsis = ast::expressions::elipsis;
            using ident   = ast::expressions::ident;

            if( fun->get_type( ) == objects::type::FUNCTION ) {


                auto vfun = objects::cast_func(fun);

                auto new_env = environment::make(vfun->env( ));
                auto new_args = objects::array::make( new_env );

                size_t id = 0;
                for( auto &p: *vfun ) {

                    if( p->get_type( ) == ast::type::IDENT ) {

                        auto n = ast::cast<ident>( p.get( ) );
                        auto v = unref( eval_impl_tail(
                                        call->param_at(id++).get( ), env ) );

                        new_env->set( n->value( ), v );

                    } else if( p->get_type( ) == ast::type::ELIPSIS ) {
                        auto eli = ast::cast<elipsis>( p.get( ) );

                        auto name = std::string( );
                        if( eli->is_ident( ) ) {
                            auto id = ast::cast<ident>( eli->value( ).get( ) );
                            name = id->value( );
                        } else {
                            name = "__args";
                        }

                        for( ; id<call->param_list( ).size( ); id++ ) {
                            auto v = unref( eval_impl_tail(
                                            call->param_at(id).get( ), env ) );
                            new_args->push( new_env.get( ), v );
                        }

                        new_env->set( name, new_args );

                        break; /// last one!
                    } else {
                        return nullptr;
                    }
                }
                return new_env;
            } else if( fun->get_type( ) == objects::type::BUILTIN ) {

                auto vfun = objects::cast_builtin(fun);
                auto new_env = environment::make(vfun->env( ));

                for( auto &cp: call->param_list( ) ) {
                    auto v = unref( eval_impl_tail( cp.get( ), env ) );
                    params.push_back( v );
                }
                return new_env;
            }
            return env;
        }

        objects::sptr create_tail_call( ast::node *n, environment::sptr env )
        {

            auto call = ast::cast<ast::expressions::call>( n );
            auto fun = eval_impl(call->func( ).get( ), env);
            if( is_null( fun ) || !is_func( fun ) ) {
                ///// TODO error call object
                return error(n, "It is not a callable object");
            }

            objects::slist params;

            auto chkd = check_args_count( call, fun.get( ), env );

            if( chkd ) {
                return is_null(chkd) ? fun : chkd;
            }

            auto new_env = create_call_env( call, fun.get( ), env, params );
            if( !new_env ) {
                return error( call, "Bad parameter for 'call'" );
            }
            return std::make_shared<objects::tail_call>( fun, std::move(params),
                                                         new_env );
        }

        static
        objects::sptr get_tail( objects::sptr obj )
        {
            if( obj->get_type( ) == objects::type::TAIL_CALL ) {
                return obj;
            } else if( obj->get_type( ) == objects::type::RETURN ) {
                auto ret = objects::cast_return( obj.get( ) );
                if( ret->value( )->get_type( ) == objects::type::TAIL_CALL ) {
                    return ret->value( );
                }
            }
            return nullptr;
        }

        objects::sptr eval_tail_return( objects::sptr obj )
        {
            using call_type = objects::tail_call;
            while( obj->get_type( ) == objects::type::TAIL_CALL ) {
                auto call = static_cast<call_type *>(obj.get( ));
                auto call_type = call->value( )->get_type( );
                if( call_type == objects::type::FUNCTION ) {
                    auto fun = objects::cast_func(call->value( ).get( ));
                    environment::scoped s( call->env( ) );
                    fun->env( )->get_state( ).GC( fun->env( ) );
                    obj = eval_impl( fun->body( ), call->env( ) );
                } else if( call_type == objects::type::BUILTIN ) {
                    auto fun = objects::cast_builtin(call->value( ).get( ));
                    fun->env( )->get_state( ).GC( fun->env( ) );
                    obj = fun->call( call->params( ), call->env( ) );
                }
                if( is_return( obj ) ) {
                    return obj;
                }
            }
            return obj;
        }

        objects::sptr eval_tail( objects::sptr obj_src )
        {
            using call_type = objects::tail_call;
            while( auto obj = get_tail( obj_src ) ) {
                auto call = static_cast<call_type *>(obj.get( ));
                auto call_type = call->value( )->get_type( );
                if( call_type == objects::type::FUNCTION ) {
                    auto fun = objects::cast_func(call->value( ).get( ));
                    environment::scoped s( call->env( ) );
                    fun->env( )->get_state( ).GC( fun->env( ) );
                    obj_src = eval_impl( fun->body( ), call->env( ) );
                } else if( call_type == objects::type::BUILTIN ) {
                    auto fun = objects::cast_builtin(call->value( ).get( ));
                    obj_src = fun->call( call->params( ), call->env( ) );
                }
            }
            return obj_src;
        }

        objects::sptr eval_scope_node( ast::node *n, environment::sptr env )
        {
            call_info::scope scp( call_stack( ), n );
            if( call_stack( )->size( ) > 2048 ) {
                return error( n, "Stack overflow ", n );
            }

            auto scope = ast::cast<ast::expressions::list>( n );
            return eval_scope( scope->value( ), env );
        }

        objects::sptr eval_scope( ast::node_list &lst, environment::sptr env )
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
                        return do_return(create_tail_call( expr->value( ),
                                                           env ) );
                    }
                }

                if( 0 == count ) {
                    if( is_function_call( stmt.get( ) ) ) {
                        return create_tail_call( stmt.get( ), env);
                    } else if( stmt->get_type( ) == ast::type::EXPR ) {
                        auto expr = static_cast<expression_type *>(stmt.get( ));
                        if( is_function_call( expr->value( ).get( ) ) ) {
                            return create_tail_call(expr->value( ).get( ), env);
                        }
                    }
                }

                auto next = eval_impl( stmt.get( ), env );
                last = next;
                if( is_return( next ) || is_fail( next ) ) {
                    return last;
                } else if( ( 0 != count ) && is_tail_call( next.get( ) ) ) {
                    last = eval_tail_return( last );
                }

                if( is_fail( last ) ) {
                    return last;
                }
            }
            return last;
        }

        objects::sptr eval_ifelse( ast::node *n, environment::sptr env )
        {
            auto ifblock = ast::cast<ast::expressions::ifelse>( n );

            for( auto &i: ifblock->ifs(  ) ) {
                auto cond = unref(eval_impl( i.cond.get( ), env ));
                if( is_fail( cond ) ) {
                    return cond;
                }
                auto res = obj2num_obj<objects::boolean>( cond.get( ) );
                if( is_null( res ) ) {
                    return error( i.cond.get( ), "Failed to convert ",
                                  cond->get_type( ), " to boolean.");
                }

                if( is_fail( res ) ) {
                    return res;
                }

                auto bres = objects::cast_bool(res.get( ));
                if( bres->value( ) ) {
                    environment::scoped s(make_env(env));
                    auto eval_states = eval_impl( i.body.get( ), s.env( ));
                    return unref(eval_states);
                }
            }
            if( ifblock->alt( ) ) {
                environment::scoped s(make_env(env));
                auto eval_states = eval_impl( ifblock->alt( ).get( ),
                                              s.env( ) );
                return unref(eval_states);
            }
            return get_null( );
        }

        objects::sptr eval_program( ast::node *n, environment::sptr env )
        {
            auto prog = ast::cast<ast::program>( n );
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
            auto expr = ast::cast<ast::statements::expr>( n );
            return eval_impl( expr->value( ).get( ), env );
        }

        objects::sptr eval_let( ast::node *n, environment::sptr env )
        {
            auto expr = ast::cast<ast::statements::let>( n );
            if( expr->ident( )->get_type( ) != ast::type::IDENT ) {
                return error(n, "Bad identifier '", expr->ident( )->str( ),
                             "' for let statement");
            }
            auto id   = expr->ident( )->str( );
            auto val  = eval_impl( expr->value( ).get( ), env );
            if( is_fail( val ) ) {
                return val;
            }
            env->set( id, unref(val) );
            return get_null( );
        }

        objects::sptr eval_return( ast::node *n, environment::sptr env )
        {
            auto expr = ast::cast<ast::statements::ret>( n );
            auto val  = eval_impl( expr->value( ), env );
            return std::make_shared<objects::retutn_obj>(val);
        }

        objects::sptr eval_ident( ast::node *n, environment::sptr env )
        {

            auto expr = ast::cast<ast::expressions::ident>( n );
            auto val = env->get( expr->value( ) );
            if( !val ) {
                return error( n, "Identifier not found '", n->str( ), "'" );
            } else {
                return val;
            }
        }

        objects::sptr eval_function( ast::node *n, environment::sptr env )
        {
            auto func = ast::cast<ast::expressions::function>( n );
            auto init_size = func->inits( ).size( );
            if( init_size > func->param_size( ) ) {
                init_size = func->param_size( );
            }
            auto fff  = objects::function::make( make_env(env),
                                                 func->params( )->clone_me( ),
                                                 func->body( )->clone( ),
                                                 init_size );

            for( auto &next: func->inits( ) ) {
                auto res = unref(eval_impl_tail( next.second.get( ), env ) );
                if( is_fail( res ) ) {
                    return res;
                }
                env->set( next.first, res );
            }
            return fff;
        }

        objects::sptr eval_index( ast::node *n, environment::sptr env )
        {
            auto idx = ast::cast<ast::expressions::index>(n);

            auto val = unref(eval_impl_tail( idx->value( ).get( ), env ));

            if( is_fail( val ) ) {
                return val;
            }

            auto idx_call = [this, env](ast::node *n ) {
                return unref( eval_impl_tail( n, env ) );
            };

            using OP_array  = operations::operation<objects::type::ARRAY>;
            using OP_string = operations::operation<objects::type::STRING>;
            using OP_table  = operations::operation<objects::type::TABLE>;

            switch ( val->get_type( ) ) {
            case objects::type::ARRAY:
                return OP_array::eval_index( idx, val, idx_call, env );
            case objects::type::STRING:
                return OP_string::eval_index( idx, val, idx_call, env );
            case objects::type::TABLE:
                return OP_table::eval_index( idx, val, idx_call, env );
            }

            return objects::error::make( n->pos( ),
                                         "Impossible to get an index of ",
                                         idx->param( )->get_type( ) );
        }

        objects::slist eval_parameters( ast::expressions::call *call,
                                        environment::sptr env )
        {
            objects::slist res;
            for( auto &e: call->param_list( ) ) {
                auto next = unref(eval_impl_tail( e.get( ), env ));
                if( is_fail(next) ) {
                    return objects::slist { next };
                }
                res.emplace_back( next );
            }
            return res;
        }

        objects::sptr check_args_count( ast::expressions::call *call,
                                        objects::base *fun,
                                        environment::sptr env )
        {
            using ident_type = ast::expressions::ident;

            if( fun->get_type( ) == objects::type::FUNCTION ) {
                auto vfun = objects::cast_func(fun);

                std::size_t call_params = call->param_list( ).size( );
                auto total = (vfun->param_size( ) - vfun->is_elipsis( ) );

                if( call_params < total ) {

                    if( call_params == 0 ) {
                        return get_null( );
                    }

                    auto new_env = environment::make(vfun->env( ));

                    for( std::size_t i = 0; i < call_params; ++i ) {

                        auto &p(*(vfun->begin( ) + i));

                        auto &c(call->param_at( i ));

                        auto ptype = p->get_type( );

                        if( ptype == ast::type::IDENT ) {
                            auto n = static_cast<ident_type *>(p.get( ));
                            auto v = unref(eval_impl_tail( c.get( ),env ) );
                            new_env->set(n->value( ), v);
                        } else {
                            return error(call, "Invalid argument ", i,
                                         p->str( ) );
                        }
                    }
                    return objects::function::make( new_env, *vfun,
                                                    call_params );
                }
            }
            return nullptr;
        }

        struct call_info {
            using uptr = std::unique_ptr<call_info>;
            using list = std::deque<uptr>;

            explicit
            call_info( ast::node *n )
                :val_(n)
            { }

            struct scope {

                scope( list *lst, ast::node *n )
                    :lst_(lst)
                {
                    lst->emplace_back( uptr(new call_info(n) ) );
                }

                ~scope( )
                {
                    lst_->pop_back( );
                }

                list *lst_;
            };

            const ast::node *val_;
        };

        call_info::list *call_stack( )
        {
            thread_local static
            call_info::list lst;

            call_info::list *res = &lst;

            return res;
        }

        objects::sptr eval_call_impl( ast::node *n,
                                      environment::sptr env,
                                      environment::sptr &/*work_env*/ )
        {
            call_info::scope scp( call_stack( ), n);
            if( call_stack( )->size( ) > 2048 ) {
                return error( n, "Stack overflow" );
            }

            auto call = ast::cast<ast::expressions::call>( n );
            auto fun = unref(eval_impl_tail(call->func( ).get( ), env));
            if( is_fail( fun ) ) {
                return fun;
            }
            if( is_null( fun ) || !is_func( fun ) ) {
                return error( call->func( ).get( ),
                              fun->get_type( ), "(", fun, ")",
                              " is not a callable object" );
            }

            if( fun->get_type( ) == objects::type::FUNCTION ) {

                auto vfun = objects::cast_func(fun.get( ));

                vfun->env( )->get_state( ).GC( vfun->env( ) );

                auto chkd = check_args_count( call, fun.get( ), env );

                if( chkd ) {
                    return is_null(chkd) ? fun : chkd;
                }

                objects::slist params;

                environment::scoped s(create_call_env( call, vfun,
                                                       env, params ) );

                if( !s.env( ) ) {
                    return get_null( );
                }

                auto res = eval_impl( vfun->body( ), s.env( ) );
                while( is_return( res ) ) {
                    auto r = objects::cast_return(res.get( ));
                    res = eval_tail( r->value( ) );
                }

                return res;

            } else if( fun->get_type( ) == objects::type::BUILTIN ) {
                environment::scoped s(make_env( env ));
                auto vfun = objects::cast_builtin(fun.get( ));

                vfun->env( )->get_state( ).GC( vfun->env( ) );

                vfun->init( s.env( ) );
                auto params = eval_parameters(call, s.env( ));
                if( params.size( ) == 1 && is_fail( params[0] ) ) {
                    return params[0];
                }
                auto res = vfun->call( params, s.env( ) );
                return res;
            }
            return error( call->func( ).get( ), "Internal error: ",
                          fun, " is function, but it is not in the call list" );
        }

        objects::sptr eval_call( ast::node *n, environment::sptr env )
        {
            environment::sptr we;
            auto res = eval_call_impl( n, env, we );
            //res = eval_tail( res );
            return res;
        }

        objects::sptr eval_array( ast::node *n, environment::sptr env )
        {
            auto arr = ast::cast<ast::expressions::array>( n );
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
            auto table = ast::cast<ast::expressions::table>( n );

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

        objects::sptr eval_registry( ast::node *n, environment::sptr env )
        {
            auto reg = ast::cast<ast::expressions::registry>(n);
            auto obj = env->get_state( ).get_registry_value( reg->value( ) );
            if( !obj ) {
                return error(n, "Registry object not found. ", reg->str( ));
            }
            return obj;
        }

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
        static
        ast::node::uptr unquote_mutator( ast::node *n, tree_walking* thiz,
                                         environment::sptr env )
        {
            namespace EXP = ast::expressions;

            if( n->get_type( ) == ast::type::UNQUOTE ) {
                auto quo = ast::cast<EXP::unquote>( n );

                auto res = thiz->eval_impl( quo, env );
                if( !is_fail( res ) ) {
                    return res->to_ast( n->pos( ) );
                } else {
                    return std::move(quo->value( ));
                }
            } else {
                n->mutate( [thiz, env](ast::node *n) {
                    return tree_walking::unquote_mutator( n, thiz, env );
                } );
            }
            return nullptr;
        }

        objects::sptr eval_quote( ast::node *n, environment::sptr env )
        {
            auto quo = ast::cast<ast::expressions::quote>(n);
            ast::node::apply_mutator( quo->value( ),
                                      [this, env]( ast::node *n ) {
                return tree_walking::unquote_mutator( n, this, env );
            } );
            return objects::quote::make( quo->value( )->clone( ) );
        }

        objects::sptr eval_unquote( ast::node *n, environment::sptr env )
        {
            auto quo = ast::cast<ast::expressions::unquote>(n);
            auto res = eval_impl_tail_ret( quo->value( ).get( ), env );
            if( res->get_type( ) == objects::type::QUOTE ) {
                auto qq = objects::cast<objects::type::QUOTE>(res.get( ));
                return eval_impl( qq->value( ).get( ), env );
            } else {
                return res;
            }
        }
#endif

        objects::sptr eval_impl_tail( ast::node *n, environment::sptr env )
        {
            auto res = eval_impl(n, env);
            return eval_tail( res );
        }

        objects::sptr eval_impl_tail_ret( ast::node *n, environment::sptr env )
        {
            auto res = eval_impl(n, env);
            return eval_tail_return( res );
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
            case ast::type::REGISTRY:
                res = eval_registry( n, env ); break;
            case ast::type::LIST:
                res = eval_scope_node( n, env ); break;

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
            case ast::type::QUOTE:
                res = eval_quote( n, env ); break;
            case ast::type::UNQUOTE:
                res = eval_unquote( n, env ); break;
#endif
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
