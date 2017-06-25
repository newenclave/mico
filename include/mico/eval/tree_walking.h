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
            return oper ? get_bool( !obj2bool( oper.get( ) ) ) : get_null( );
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

        template <typename Res>
        Res obj2num( objects::base *oper )
        {
            switch (oper->get_type( )) {
            case objects::type::BOOLEAN: {
                auto val = static_cast<objects::boolean *>(oper)->value( )
                         ? 1 : 0;
                return static_cast<Res>( val );
            }
            case objects::type::UNSIGNED_INT: {
                auto val = static_cast<objects::unsigned_int *>(oper)->value( );
                return static_cast<Res>( val );
            }
            case objects::type::SIGNED_INT: {
                auto val = static_cast<objects::signed_int *>(oper)->value( );
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
                return std::make_shared<NumObj>( obj2num<bool>(oper) );
            case objects::type::UNSIGNED_INT:
                return std::make_shared<NumObj>( obj2num<std::uint64_t>(oper) );
            case objects::type::SIGNED_INT:
                return std::make_shared<NumObj>( obj2num<std::int64_t>(oper) );
            case objects::type::FLOAT:
                return std::make_shared<NumObj>( obj2num<double>(oper) );
            default:
                break;
            }
            return nullptr;
        }

        template <typename ResT, typename NumericT>
        objects::sptr get_num_infix( NumericT lft, NumericT rgth,
                                     tokens::type oper )
        {
            switch (oper) {
            case tokens::type::PLUS:
                return std::make_shared<ResT>( lft + rgth );
            case tokens::type::MINUS:
                return std::make_shared<ResT>( lft - rgth );
            case tokens::type::ASTERISK:
                return std::make_shared<ResT>( lft * rgth );
            case tokens::type::SLASH:
                return std::make_shared<ResT>( lft / rgth );
            default:
                break;
            }
            return nullptr;
        }

        objects::sptr get_infix( ast::node *n )
        {
            auto inf = static_cast<ast::expressions::infix *>(n);
            auto left = eval(inf->left( ));
            if( !left ) {
                /////////// bad left value
                return nullptr;
            }

            switch (left->get_type( )) {
            case objects::type::SIGNED_INT: {
                auto lval = obj2num<std::int64_t>(left.get( ));
                auto right = eval(inf->right( ));
                if( !right ) {
                    /////////// bad right value
                    return nullptr;
                }
                auto robj = obj2num_obj<objects::signed_int>(right.get( ));
                if( !robj ) {
                    /////////// bad right value type
                    return nullptr;
                }
                return get_num_infix<objects::signed_int>( lval,
                       obj2num<std::int64_t>(robj.get( )), inf->token( ));
            }
            case objects::type::UNSIGNED_INT: {
                auto lval = obj2num<std::uint64_t>(left.get( ));
                auto right = eval(inf->right( ));
                if( !right ) {
                    /////////// bad right value
                    return nullptr;
                }
                auto robj = obj2num_obj<objects::unsigned_int>(right.get( ));
                if( !robj ) {
                    /////////// bad right value type
                    return nullptr;
                }
                return get_num_infix<objects::unsigned_int>( lval,
                       obj2num<std::uint64_t>(robj.get( )), inf->token( ));
            }
            case objects::type::FLOAT: {
                auto lval = obj2num<double>(left.get( ));
                auto right = eval(inf->right( ));
                if( !right ) {
                    /////////// bad right value
                    return nullptr;
                }
                auto robj = obj2num_obj<objects::floating>(right.get( ));
                if( !robj ) {
                    /////////// bad right value type
                    return nullptr;
                }
                return get_num_infix<objects::floating>( lval,
                       obj2num<double>(robj.get( )), inf->token( ));
            }
            default:
                break;
            }

            if( inf->token( ) == tokens::type::PLUS ) {
                if( left->get_type( ) == objects::type::STRING ) {
                    auto rght = eval(inf->right( ));
                    if( rght->get_type( ) == objects::type::STRING ) {
                        auto lval = static_cast<objects::string *>(left.get( ));
                        auto rval = static_cast<objects::string *>(rght.get( ));
                        return std::make_shared<objects::string>(
                                    lval->value( ) + rval->value( ));
                    }
                }
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
            case ast::type::INFIX:
                return get_infix( n );
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
