#ifndef MICO_TREE_WALKING_H
#define MICO_TREE_WALKING_H

#include "mico/eval/evaluator.h"
#include "mico/expressions.h"
#include "mico/statements.h"
#include "mico/tokens.h"

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

        static
        objects::sptr get_bool_true( )
        {
            static auto bobj = std::make_shared<objects::boolean>(true);
            return bobj;
        }

        static
        objects::sptr get_bool_false( )
        {
            static auto bobj = std::make_shared<objects::boolean>(false);
            return bobj;
        }

        objects::sptr get_bool( ast::node *n )
        {
            auto bstate = static_cast<ast::expressions::boolean *>(n);
            return bstate->value( ) ? get_bool_true( ) : get_bool_false( );
        }

        objects::sptr get_bool( bool b )
        {
            return b ? get_bool_true( ) : get_bool_false( );
        }

        objects::unsigned_int::sptr get_int( ast::node *n )
        {
            auto state = static_cast<ast::expressions::integer *>(n);
            return std::make_shared<objects::unsigned_int>( state->value( ) );
        }

        objects::signed_int::sptr get_signed( ast::node *n )
        {
            auto state = static_cast<ast::expressions::integer *>(n);
            auto sign_value = static_cast<std::int64_t>(state->value( ));
            return std::make_shared<objects::signed_int>( sign_value );
        }

        objects::signed_int::sptr get_signed( std::int64_t n )
        {
            return std::make_shared<objects::signed_int>( n );
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

        objects::sptr produce_prefix_minus( ast::expressions::prefix *n )
        {
            auto oper = eval(n->value( ));
            if( oper->get_type( ) == objects::type::SIGNED_INT ) {
                auto o = static_cast<objects::unsigned_int *>(oper.get( ));
                return get_signed( -1 * static_cast<std::int64_t>(o->value( )));
            } else if( oper->get_type( ) == objects::type::UNSIGNED_INT ) {
                auto o = static_cast<objects::signed_int *>(oper.get( ));
                return get_signed( -1 * o->value( ) );
            } else if( oper->get_type( ) == objects::type::FLOAT ) {
                auto o = static_cast<objects::floating *>(oper.get( ));
                return get_float( -1.0 * o->value( ) );
            } else {
                //// operation invalid
            }
            return nullptr;
        }

        bool is_bool( objects::base *oper )
        {
            switch (oper->get_type( ) ) {
            case objects::type::SIGNED_INT:
            case objects::type::UNSIGNED_INT:
            case objects::type::FLOAT:
            case objects::type::BOOLEAN:
            case objects::type::NULL_OBJ:
            case objects::type::STRING:
                return true;
            }
            return false;
        }

        bool obj2bool( objects::base *oper )
        {
            if( oper->get_type( ) == objects::type::SIGNED_INT ) {
                auto o = static_cast<objects::unsigned_int *>(oper);
                return o->value( ) != 0;
            } else if( oper->get_type( ) == objects::type::UNSIGNED_INT ) {
                auto o = static_cast<objects::unsigned_int *>(oper);
                return o->value( ) != 0;
            } else if( oper->get_type( ) == objects::type::FLOAT ) {
                auto o = static_cast<objects::unsigned_int *>(oper);
                return o->value( ) != 0.0;
            } else if( oper->get_type( ) == objects::type::BOOLEAN ) {
                auto o = static_cast<objects::boolean *>(oper);
                return o->value( );
            } if( oper->get_type( ) == objects::type::STRING ) {
                auto o = static_cast<objects::string *>(oper);
                return !o->value( ).empty( );
            } if( oper->get_type( ) == objects::type::NULL_OBJ ) {
                return false;
            } else {
                //// operation invalid
            }
            return true; // ?
        }

        objects::sptr produce_prefix_bang( ast::expressions::prefix *n )
        {
            auto oper = eval(n->value( ));
            return oper ? get_bool( obj2bool( oper.get( ) ) ) : get_null( );
        }

        objects::sptr get_prefix( ast::node *n )
        {
            auto expr = static_cast<ast::expressions::prefix *>( n );

            switch ( expr->token( ) ) {
            case tokens::type::MINUS:
                return produce_prefix_minus( expr );
            case tokens::type::BANG:
                return produce_prefix_bang( expr );
            default:
                break;
            }

            return nullptr;
        }

        objects::sptr get_expression( ast::node *n )
        {
            auto expr = static_cast<ast::statements::expr *>( n );
            return eval( expr->value( ).get( ) );
        }

    public:

        objects::sptr eval( ast::node *n ) override
        {
            switch (n->get_type( )) {
            case ast::type::EXPR:
                return get_expression( n );
            case ast::type::BOOLEAN:
                return get_bool( n );
            case ast::type::INTEGER:
                return get_int( n );
            case ast::type::FLOAT:
                return get_float( n );
            case ast::type::STRING:
                return get_string( n );
            case ast::type::PREFIX:
                return get_prefix( n );
            default:
                break;
            }
            return get_null( );
        }

    private:
        error_list errors_;
    };

}}

#endif // TREE_WALKING_H
