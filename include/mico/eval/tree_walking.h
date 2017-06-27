#ifndef MICO_TREE_WALKING_H
#define MICO_TREE_WALKING_H

#include "mico/eval/evaluator.h"
#include "mico/expressions.h"
#include "mico/statements.h"
#include "mico/tokens.h"
#include "mico/enviroment.h"

namespace mico { namespace eval {

    class tree_walking: public base {

    public:
        using error_list = std::vector<std::string>;
    private:


        ////////////// errors /////////////

        void error_operation_notfound( tokens::type tt, const objects::base *a )
        {

        }

        //////////////////////////////////
        static
        objects::sptr get_null( )
        {
            static auto nobj = std::make_shared<objects::null>( );
            return nobj;
        }

        template <typename ToT, typename FromT>
        ToT *obj_cast( FromT *obj )
        {
#ifdef DEBUG
            if(objects::object2type<ToT>::type_name == obj->get_type( )) {
                throw std::runtime_error("Invalid type conversion;");
            }
#endif
            return static_cast<ToT *>(obj);
        }

        static
        bool is_null( const objects::sptr &obj )
        {
            return (!obj) || (obj->get_type( ) == objects::type::NULL_OBJ);
        }

        static
        objects::boolean::sptr get_bool_true( )
        {
            static auto bobj = std::make_shared<objects::boolean>(true);
            return bobj;
        }

        static
        objects::boolean::sptr get_bool_false( )
        {
            static auto bobj = std::make_shared<objects::boolean>(false);
            return bobj;
        }

        objects::boolean::sptr eval_bool( ast::node *n )
        {
            auto bstate = static_cast<ast::expressions::boolean *>(n);
            return bstate->value( ) ? get_bool_true( ) : get_bool_false( );
        }

        objects::boolean::sptr eval_bool( bool b )
        {
            return b ? get_bool_true( ) : get_bool_false( );
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
                                            enviroment::sptr env )
        {
            auto oper = eval_impl(n->value( ), env);
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
            switch (oper->get_type( ) ) {
            case objects::type::INTEGER:
            case objects::type::FLOAT:
            case objects::type::BOOLEAN:
            case objects::type::NULL_OBJ:
                return true;
            }
            return false;
        }

        bool is_return( objects::sptr obj )
        {
            return obj->get_type( ) == objects::type::RETURN;
        }

        bool is_function( objects::sptr obj )
        {
            return obj->get_type( ) == objects::type::FUNCTION;
        }

        objects::sptr produce_prefix_bang( ast::expressions::prefix *n,
                                           enviroment::sptr env )
        {
            auto oper = eval_impl( n->value( ), env );
            return oper ? eval_bool( !obj2num<bool>( oper.get( ) ) )
                        : get_null( );
        }

        objects::sptr eval_prefix( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::expressions::prefix *>( n );

            switch ( expr->token( ) ) {
            case tokens::type::MINUS:
                return produce_prefix_minus( expr, env );
            case tokens::type::BANG:
                return produce_prefix_bang( expr, env );
            default:
                break;
            }

            return nullptr;
        }

        template <typename Res>
        Res obj2num( objects::base *oper )
        {
            switch (oper->get_type( )) {
            case objects::type::BOOLEAN: {
                auto val = static_cast<objects::boolean *>(oper)->value( )
                         ? 1 : 0;
                return static_cast<Res>( val );
            }
            case objects::type::INTEGER: {
                auto val = static_cast<objects::integer *>(oper)->value( );
                return static_cast<Res>( val );
            }
            case objects::type::FLOAT: {
                auto val = static_cast<objects::floating *>(oper)->value( );
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
                return eval_bool( obj2num<bool>(oper) );
            case objects::type::INTEGER:
                return std::make_shared<NumObj>( obj2num<std::int64_t>(oper) );
            case objects::type::FLOAT:
                return std::make_shared<NumObj>( obj2num<double>(oper) );
            default:
                break;
            }
            return get_null( );
        }

        template <typename ResT, typename NumericT>
        objects::sptr get_num_infix( NumericT lft, NumericT rghg,
                                     tokens::type oper )
        {
            switch (oper) {
            case tokens::type::PLUS:
                return std::make_shared<ResT>( lft +  rghg );
            case tokens::type::MINUS:
                return std::make_shared<ResT>( lft -  rghg );
            case tokens::type::ASTERISK:
                return std::make_shared<ResT>( lft *  rghg );
            case tokens::type::SLASH:
                return std::make_shared<ResT>( lft /  rghg );
            case tokens::type::LT:
                return               eval_bool( lft <  rghg );
            case tokens::type::GT:
                return               eval_bool( lft >  rghg );
            case tokens::type::EQ:
                return               eval_bool( lft == rghg );
            case tokens::type::NOT_EQ:
                return               eval_bool( lft != rghg );
            case tokens::type::LT_EQ:
                return               eval_bool( lft <= rghg );
            case tokens::type::GT_EQ:
                return               eval_bool( lft >= rghg );
            default:
                break;
            }
            return get_null( );
        }

        objects::sptr compare_bool( bool lft, bool rght, tokens::type tt )
        {
            switch (tt) {
            case tokens::type::LT:
                return eval_bool( lft < rght );
            case tokens::type::GT:
                return eval_bool( lft > rght );
            case tokens::type::LT_EQ:
                return eval_bool( lft <= rght );
            case tokens::type::GT_EQ:
                return eval_bool( lft >= rght );
            case tokens::type::EQ:
                return eval_bool( lft == rght );
            case tokens::type::NOT_EQ:
                return eval_bool( lft != rght );
            default:
                break;
            }
            /// invalid operation for boollean;
            return get_null( );
        }

        enviroment::sptr make_env( enviroment::sptr parent )
        {
            return enviroment::make(parent);
        }

        objects::sptr infix_string( const std::string &lft,
                                    const std::string &rght,
                                    tokens::type tt )
        {
            switch (tt) {
            case tokens::type::LT:
                return eval_bool( lft < rght );
            case tokens::type::GT:
                return eval_bool( lft > rght );
            case tokens::type::LT_EQ:
                return eval_bool( lft <= rght );
            case tokens::type::GT_EQ:
                return eval_bool( lft >= rght );
            case tokens::type::EQ:
                return eval_bool( lft == rght );
            case tokens::type::NOT_EQ:
                return eval_bool( lft != rght );
            case tokens::type::PLUS:
                return std::make_shared<objects::string>(lft + rght);
            default:
                break;
            }
            /// invalid operation for string;
            return get_null( );
        }

        objects::sptr other_infix( objects::base *lft, objects::base *rgh,
                                   tokens::type tt, enviroment::sptr /*env*/ )
        {
            switch ( lft->get_type( ) ) {
            case objects::type::STRING: {
                if( rgh->get_type( ) == objects::type::STRING ) {
                    auto lval = static_cast<objects::string *>(lft);
                    auto rval = static_cast<objects::string *>(rgh);
                    return infix_string( lval->value( ), rval->value( ), tt );
                } else {
                    /// TODO: error bad operation for STRING and ...
                    return get_null( );
                }
            }
            case objects::type::BOOLEAN: {
                auto lval = static_cast<objects::boolean *>(lft);
                auto rval = obj2num_obj<objects::boolean>( rgh );
                if( !is_null( rval ) ) {
                    auto bval = static_cast<objects::boolean *>(rval.get( ));
                    return compare_bool( lval->value( ), bval->value( ), tt );
                } else {
                    /// TODO: error bad operation for BOOLEAN and ...
                    return get_null( );
                }
            }
            default:
                break;
            }
            return get_null( );
        }

        objects::sptr eval_infix( ast::node *n, enviroment::sptr env )
        {
            auto inf = static_cast<ast::expressions::infix *>(n);
            auto left = eval_impl(inf->left( ), env);
            if( !left ) {
                /////////// bad left value
                return get_null( );
            }

            switch (left->get_type( )) {
            case objects::type::INTEGER: {
                auto lval = obj2num<std::int64_t>(left.get( ));
                auto right = eval_impl(inf->right( ), env);
                if( !right ) {
                    /////////// bad right value
                    return get_null( );;
                }
                auto robj = obj2num_obj<objects::integer>(right.get( ));
                if( !robj ) {
                    /////////// bad right value type
                    return get_null( );;
                }
                return get_num_infix<objects::integer>( lval,
                            obj2num<std::int64_t>(robj.get( )), inf->token( ));
            }
            case objects::type::FLOAT: {
                auto lval = obj2num<double>(left.get( ));
                auto right = eval_impl(inf->right( ), env);
                if( !right ) {
                    /////////// bad right value
                    return get_null( );;
                }
                auto robj = obj2num_obj<objects::floating>(right.get( ));
                if( !robj ) {
                    /////////// bad right value type
                    return get_null( );;
                }
                return get_num_infix<objects::floating>( lval,
                       obj2num<double>(robj.get( )), inf->token( ));
            }
            default:
                break;
            }

            switch ( inf->token( ) ) {
            case tokens::type::LT:
            case tokens::type::PLUS:
            case tokens::type::GT:
            case tokens::type::LT_EQ:
            case tokens::type::GT_EQ:
            case tokens::type::EQ:
            case tokens::type::NOT_EQ: {
                auto rght = eval_impl(inf->right( ), env);
                return other_infix( left.get( ), rght.get( ),
                                    inf->token( ), env );
            }
            default:
                break;
            }

            return get_null( );
        }

        enviroment::sptr create_call_env( ast::expressions::call *call,
                                          objects::base *fun,
                                          enviroment::sptr env )
        {
            auto vfun = obj_cast<objects::function>(fun);

            auto new_env = enviroment::make(vfun->env( ));

            if( call->params( ).size( ) != vfun->params( ).size( ) ) {
                //// TODO bad params count
                return nullptr;
            }

            size_t id = 0;
            for( auto &p: vfun->params( ) ) {
                if( p->get_type( ) == ast::type::IDENT ) {
                    auto n = static_cast<ast::expressions::ident *>(p.get( ));
                    auto v = eval_impl_tail(call->params( )[id++].get( ), env );
                    new_env->set(n->value( ), v);
                } else {
                    /// TODO bad param
                    return nullptr;
                }
            }
            return new_env;
        }

        objects::sptr get_cont_call( ast::node *n, enviroment::sptr env )
        {
            auto call = static_cast<ast::expressions::call *>( n );
            auto fun = eval_impl(call->func( ), env);
            if( is_null( fun ) || !is_function( fun ) ) {
                ///// TODO error call object
                return get_null( );
            }

            auto new_env = create_call_env( call, fun.get( ), env );
            if( !new_env ) {
                return get_null( );
            }
            return std::make_shared<objects::cont_call>(fun, new_env);
        }


        objects::sptr eval_tail( objects::sptr obj )
        {
            using call_type = objects::cont_call;
            while(obj->get_type( ) == objects::type::CONT_CALL ) {
                auto call = static_cast<call_type *>(obj.get( ));
                auto fun = obj_cast<objects::function>(call->value( ).get( ));
                obj = eval_scope_impl( fun->body( ), call->env( ) );
            }
            return obj;
        }

        objects::sptr eval_scope( ast::statement_list &lst,
                                  enviroment::sptr env )
        {
            objects::sptr res = eval_scope_impl(lst, env);
            return eval_tail( res );
        }

        objects::sptr eval_scope_impl( ast::statement_list &lst,
                                       enviroment::sptr env )
        {
            using return_type = ast::statements::ret;
            using expression_type = ast::statements::expr;
            auto count = lst.size( );
            objects::sptr last = get_null( );
            for( auto &stmt: lst ) {

                --count;

                if(stmt->get_type( ) == ast::type::RETURN) {
                    auto expr = static_cast<return_type *>( stmt.get( ) );
                    if( expr->value( )->get_type( ) == ast::type::CALL ) {
                        return get_cont_call( expr->value( ), env );
                    }
                }

                if( 0 == count ) {
                    if(stmt->get_type( ) == ast::type::EXPR) {
                        auto expr = static_cast<expression_type *>(stmt.get( ));
                        if( expr->value( )->get_type( ) == ast::type::CALL ) {
                            return get_cont_call(expr->value( ).get( ), env);
                        }
                    }
                }

                auto next = eval_impl( stmt.get( ), env );
                last = next;
                if( is_return( next ) ) {
                    return extract_return( last );
                }
            }
            return last;
        }

        objects::sptr eval_ifelse( ast::node *n, enviroment::sptr env )
        {
            auto ifblock = static_cast<ast::expressions::if_expr *>( n );

            for( auto &i: ifblock->ifs(  ) ) {
                auto cond = eval_impl( i.cond.get( ), env );
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
                    auto eval_states = eval_scope_impl( i.states,
                                                        make_env(env) );
                    return eval_states;
                }
            }
            if( !ifblock->alt( ).empty( ) ) {
                auto eval_states = eval_scope_impl( ifblock->alt( ),
                                                    make_env(env) );
                return eval_states;
            }
            return get_null( );
        }

        objects::sptr eval_program( ast::node *n, enviroment::sptr env )
        {
            auto prog = static_cast<ast::program *>( n );
            objects::sptr last = get_null( );
            for( auto &s: prog->states( ) ) {
                last = eval_impl_tail( s.get( ), env );
                if( is_return(last) ) {
                    return extract_return( last );
                }
            }
            return last;
        }

        objects::sptr eval_expression( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::statements::expr *>( n );
            return eval_impl( expr->value( ).get( ), env );
        }

        objects::sptr eval_let( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::statements::let *>( n );
            auto id   = expr->ident( )->str( );
            auto val  = eval_impl( expr->value( ), env );
            env->set( id, val );
            return get_null( );
        }

        objects::sptr eval_return( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::statements::ret *>( n );
            auto val  = eval_impl( expr->value( ), env );
            return std::make_shared<objects::retutn_obj>(val);
        }

        objects::sptr eval_ident( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::expressions::ident *>( n );
            auto val = env->get( expr->value( ) );
            if( !val ) {
                //// TODO error identifier
                ///
                //std::cout << expr->value( )  << " NOT FOUND!\n";
                return get_null( );
            } else {
                return val;
            }
        }

        objects::sptr eval_function( ast::node *n, enviroment::sptr env )
        {
            auto func = static_cast<ast::expressions::function *>( n );
            auto fff  = std::make_shared<objects::function>( env,
                                              func->ident_ptr( ),
                                              func->expr_ptr( ));

            return fff;
        }

        objects::sptr eval_call( ast::node *n, enviroment::sptr env )
        {
            auto call = static_cast<ast::expressions::call *>( n );
            auto fun = eval_impl(call->func( ), env);
            if( is_null( fun ) || !is_function( fun ) ) {
                ///// TODO error call object
                return get_null( );
            }

            auto vfun = obj_cast<objects::function>(fun.get( ));
            auto new_env = create_call_env( call, fun.get( ), env );
            if( !new_env ) {
                return get_null( );
            }

            auto res = eval_scope( vfun->body( ), new_env );

            return res;
        }

        objects::sptr eval_table( ast::node *n, enviroment::sptr env )
        {
            auto table = static_cast<ast::expressions::table *>( n );

            auto res = objects::table::make( );

            for( auto &v: table->value( ) ) {
                auto key = eval_impl_tail( v.first.get( ), env );
                if( is_null( key ) ) {
                    //// TODO bad key value for table
                    return get_null( );
                }
                auto val = eval_impl_tail( v.second.get( ), env );
                res->value( )[key] = val;
            }

            return res;
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

        objects::sptr eval_impl_tail( ast::node *n, enviroment::sptr env )
        {
            auto res = eval_impl(n, env);
            return eval_tail( res );
        }

        objects::sptr eval_impl( ast::node *n, enviroment::sptr env )
        {
            objects::sptr res = get_null( );
            switch (n->get_type( )) {
            case ast::type::PROGRAM:
                res = eval_program( n, env ); break;
            case ast::type::EXPR:
                res = eval_expression( n, env ); break;
            case ast::type::BOOLEAN:
                res = eval_bool( n ); break;
            case ast::type::INTEGER:
                res = eval_int( n ); break;
            case ast::type::FLOAT:
                res = eval_float( n ); break;
            case ast::type::STRING:
                res = eval_string( n ); break;
            case ast::type::PREFIX:
                res = eval_prefix( n, env ); break;
            case ast::type::INFIX:
                res = eval_infix( n, env ); break;
            case ast::type::IFELSE:
                res = eval_ifelse( n, env ); break;
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

        objects::sptr eval( ast::node *n, enviroment::sptr env ) override
        {
            return eval_impl( n, env );
        }

    private:
        error_list errors_;
    };

}}

#endif // TREE_WALKING_H
