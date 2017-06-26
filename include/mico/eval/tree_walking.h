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

        objects::boolean::sptr get_bool( ast::node *n )
        {
            auto bstate = static_cast<ast::expressions::boolean *>(n);
            return bstate->value( ) ? get_bool_true( ) : get_bool_false( );
        }

        objects::boolean::sptr get_bool( bool b )
        {
            return b ? get_bool_true( ) : get_bool_false( );
        }

        objects::integer::sptr get_int( ast::node *n )
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

        objects::sptr get_float( ast::node *n )
        {
            auto state = static_cast<ast::expressions::floating *>(n);
            return std::make_shared<objects::floating>( state->value( ) );
        }

        objects::floating::sptr get_float( double n )
        {
            return std::make_shared<objects::floating>( n );
        }

        objects::string::sptr get_string( ast::node *n )
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
                return get_float( -1.0 * o->value( ) );
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
            return oper ? get_bool( !obj2num<bool>( oper.get( ) ) )
                        : get_null( );
        }

        objects::sptr get_prefix( ast::node *n, enviroment::sptr env )
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
                return               get_bool( lft <  rghg );
            case tokens::type::GT:
                return               get_bool( lft >  rghg );
            case tokens::type::EQ:
                return               get_bool( lft == rghg );
            case tokens::type::NOT_EQ:
                return               get_bool( lft != rghg );
            case tokens::type::LT_EQ:
                return               get_bool( lft <= rghg );
            case tokens::type::GT_EQ:
                return               get_bool( lft >= rghg );
            default:
                break;
            }
            return get_null( );
        }

        objects::sptr compare_bool( bool lft, bool rght, tokens::type tt )
        {
            switch (tt) {
            case tokens::type::LT:
                return get_bool( lft < rght );
            case tokens::type::GT:
                return get_bool( lft > rght );
            case tokens::type::LT_EQ:
                return get_bool( lft <= rght );
            case tokens::type::GT_EQ:
                return get_bool( lft >= rght );
            case tokens::type::EQ:
                return get_bool( lft == rght );
            case tokens::type::NOT_EQ:
                return get_bool( lft != rght );
            default:
                break;
            }
            /// invalid operation for boollean;
            return get_null( );
        }

        objects::sptr infix_string( const std::string &lft,
                                    const std::string &rght,
                                    tokens::type tt )
        {
            switch (tt) {
            case tokens::type::LT:
                return get_bool( lft < rght );
            case tokens::type::GT:
                return get_bool( lft > rght );
            case tokens::type::LT_EQ:
                return get_bool( lft <= rght );
            case tokens::type::GT_EQ:
                return get_bool( lft >= rght );
            case tokens::type::EQ:
                return get_bool( lft == rght );
            case tokens::type::NOT_EQ:
                return get_bool( lft != rght );
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

        objects::sptr get_infix( ast::node *n, enviroment::sptr env )
        {
            auto inf = static_cast<ast::expressions::infix *>(n);
            auto left = eval_impl(inf->left( ), env);
            if( !left ) {
                /////////// bad left value
                return nullptr;
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
                return other_infix( left.get( ), rght.get( ), inf->token( ),
                                    env );
            }
            default:
                break;
            }

            return get_null( );
        }

        objects::sptr eval_scope( ast::statement_list &lst,
                                  enviroment::sptr env )
        {
            objects::sptr last = get_null( );
            for( auto &s: lst ) {
                last = eval_impl( s.get( ), env );
            }
            return last;
        }

        objects::sptr get_ifelse( ast::node *n, enviroment::sptr env )
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
                    auto eval_states = eval_scope( i.states, env );
                    return eval_states;
                }
            }
            if( !ifblock->alt( ).empty( ) ) {
                auto eval_states = eval_scope( ifblock->alt( ), env );
                return eval_states;
            }
            return get_null( );
        }

        objects::sptr get_program( ast::node *n, enviroment::sptr env )
        {
            auto prog = static_cast<ast::program *>( n );
            objects::sptr last = get_null( );
            for( auto &s: prog->states( ) ) {
                last = eval_impl( s.get( ), env );
                if( is_return(last) ) {
                    return extract_return( last );
                }
            }
            return last;
        }

        objects::sptr get_expression( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::statements::expr *>( n );
            return eval_impl( expr->value( ).get( ), env );
        }

        objects::sptr get_let( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::statements::let *>( n );
            auto id   = expr->ident( )->str( );
            auto val  = eval_impl( expr->value( ), env );
            env->set( id, val );
            return get_null( );
        }

        objects::sptr get_return( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::statements::ret *>( n );
            auto val  = eval_impl( expr->value( ), env );
            return std::make_shared<objects::retutn_obj>(val);
        }

        objects::sptr get_ident( ast::node *n, enviroment::sptr env )
        {
            auto expr = static_cast<ast::expressions::ident *>( n );
            auto val = env->get( expr->value( ) );
            if( !val ) {
                //// TODO error identifier
                ///
                //std::cout << expr->value( )  << " NOt FOUND!\n";
                return get_null( );
            } else {
                return val;
            }
        }

        objects::sptr get_function( ast::node *n, enviroment::sptr env )
        {
            auto func = static_cast<ast::expressions::function *>( n );
            auto fff  = std::make_shared<objects::function>( env,
                                              func->ident_ptr( ),
                                              func->expr_ptr( ));

            return fff;
        }

        objects::sptr get_call( ast::node *n, enviroment::sptr env )
        {
            auto call = static_cast<ast::expressions::call *>( n );
            auto fun = eval_impl(call->func( ), env);
            if( is_null( fun ) || !is_function( fun ) ) {
                ///// TODO error call object
                return get_null( );
            }

            auto vfun = obj_cast<objects::function>(fun.get( ));
            auto new_env = std::make_shared<enviroment>(vfun->env( ));
            if( call->params( ).size( ) != vfun->params( ).size( ) ) {
                //// TODO bad params count
            }

            size_t id = 0;
            for( auto &p: vfun->params( ) ) {
                if( p->get_type( ) == ast::type::IDENT ) {
                    auto n = static_cast<ast::expressions::ident *>(p.get( ));
                    auto v = eval_impl( call->params( )[id++].get( ), env );
                    new_env->set(n->value( ), v);
                } else {
                    /// TODO bad param
                    return get_null( );
                }
            }

            objects::sptr last = get_null( );
            for( auto &stmt: vfun->body( ) ) {
                auto next = eval_impl( stmt.get( ), new_env );
                last = next;
                if( is_return( next ) ) {
                    return extract_return( last );
                }
            }

            return last;
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

        objects::sptr eval_impl( ast::node *n, enviroment::sptr env )
        {
            switch (n->get_type( )) {
            case ast::type::PROGRAM:
                return get_program( n, env );
            case ast::type::EXPR:
                return get_expression( n, env );
            case ast::type::BOOLEAN:
                return get_bool( n );
            case ast::type::INTEGER:
                return get_int( n );
            case ast::type::FLOAT:
                return get_float( n );
            case ast::type::STRING:
                return get_string( n );
            case ast::type::PREFIX:
                return get_prefix( n, env );
            case ast::type::INFIX:
                return get_infix( n, env );
            case ast::type::IFELSE:
                return get_ifelse( n, env );
            case ast::type::IDENT:
                return get_ident( n, env );
            case ast::type::LET:
                return get_let( n, env );
            case ast::type::RETURN:
                return get_return( n, env );
            case ast::type::FN:
                return get_function( n, env );
            case ast::type::CALL:
                return get_call( n, env );
            case ast::type::NONE:
                break;
            }
            return get_null( );

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