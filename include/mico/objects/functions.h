#ifndef MICO_OBJECTS_FUNCTIONS_H
#define MICO_OBJECTS_FUNCTIONS_H

#include "mico/objects/reference.h"
#include "mico/objects/collectable.h"
#include "mico/objects/null.h"

#include "mico/expressions/fn.h"
#include "mico/expressions/list.h"

#include "mico/state.h"

namespace mico { namespace objects {

    template <>
    class impl<type::FUNCTION>: public collectable<type::FUNCTION> {
        using this_type = impl<type::FUNCTION>;
    public:
        static const type type_value = type::FUNCTION;
        using sptr = std::shared_ptr<this_type>;

        using param_iterator = ast::node_list::iterator;

        using body_type = ast::node;
        using body_ptr  = body_type::sptr;

        using param_type = ast::expressions::list;
        using param_ptr  = param_type::sptr;

        impl( std::shared_ptr<environment> e,
                 param_type::uptr par,
                 body_type::uptr body,
                 std::size_t start = 0)
            :collectable(e)
            ,params_(par.release( ))
            ,body_(body.release( ))
            ,start_param_(start)
        { }

        impl( std::shared_ptr<environment> e,
                 param_ptr par,
                 body_ptr body,
                 std::size_t start = 0)
            :collectable(e)
            ,params_(par)
            ,body_(body)
            ,start_param_(start)
        { }

        ~impl( )
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            if( start_param_ == 0  ) {
                oss << "fn(" << param_size( ) << ")";
            } else {
                oss << "fn(" << param_size( ) << ":"
                    << params_->value( ).size( ) << ")";
            }
            return oss.str( );
        }

        static
        sptr make( std::shared_ptr<environment> e,
                   param_type::uptr par,
                   ast::node::uptr body,
                   std::size_t start = 0 )
        {
            return std::make_shared<impl>( e, std::move(par), std::move(body),
                                           start );
        }

        static
        sptr make( std::shared_ptr<environment> e,
                   this_type &other, std::size_t start )
        {
            return std::make_shared<impl>( e, other.params_,
                                              other.body_,
                                              start + other.start_param_ );
        }

        static
        sptr make_from_partial( this_type::sptr other )
        {
            if( other->start_param_ != 0 ) {
                if( auto p = other->env( ) ) {
                    return std::make_shared<impl>( p->parent( ),
                                                      other->params_,
                                                      other->body_, 0 );
                }
            }
            return other;
        }

        bool is_part( ) const
        {
            return start_param_ != 0;
        }

        param_iterator begin( )
        {
            return params_->value( ).begin( ) + start_param_;
        }

        param_iterator end( )
        {
            return params_->value( ).end( );
        }

        std::size_t param_size( ) const
        {
            return params_->value( ).size( ) - start_param_;
        }

        const ast::node *body( ) const
        {
            return body_.get( );
        }

        ast::node *body( )
        {
            return body_.get( );
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<this_type>( env( ), params_, body_,
                                                start_param_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::FN>;
            auto res = ast::node::make<ast_type>(pos);

            auto e = env( );
            res->set_body( body_->clone( ) );
            res->set_params( params_->clone_me( ) );

            for( std::size_t i = 0; i < start_param_; ++i ) {

                auto &p( params_->value( )[i] );

                if( p->get_type( ) == ast::type::IDENT ) {
                    auto id = static_cast<ast::expressions::ident *>(p.get( ));
                    auto obj = e->get( id->value( ) );
                    if( !obj ) {
                        throw std::logic_error( "Invalid object." );
                    }
                    auto ast_res = obj->to_ast( pos );
                    if( !ast_res->is_expression( ) ) {
                        throw std::logic_error( "Invalid AST." );
                    }
                    auto expr = ast::expression::cast( ast_res );
                    res->inits( ).emplace( std::make_pair(id->value( ),
                                                     std::move( expr) ) );
                } else {
                    throw std::logic_error( "Not valid type for ident." );
                }
            }
            return res;
        }

    private:

        param_ptr   params_;
        body_ptr    body_;
        std::size_t start_param_ = 0;
    };

    template <>
    class impl<type::BUILTIN>: public collectable<type::BUILTIN> {
        using this_type = impl<type::BUILTIN>;
    public:
        static const type type_value = type::BUILTIN;

        using sptr = std::shared_ptr<this_type>;

        impl( std::shared_ptr<environment> e )
            :collectable(e)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(" << static_cast<const void *>(this) << ")";
            return oss.str( );
        }

        virtual
        objects::sptr call( objects::slist &, environment::sptr )
        {
            return impl<type::NULL_OBJ>::make( );
        }

        bool equal( const objects::base *other ) const override
        {
            if( get_type( ) == other->get_type( ) ) {
                auto ot = static_cast<const this_type *>(other);
                return ot == this;
            }
            return false;
        }

        virtual
        std::uintptr_t stub_number( ) const = 0;

        virtual
        void init( environment::sptr )
        { }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::REGISTRY>;
            std::uintptr_t id = 0;
            if( auto p = env( ) ) {
                auto cl = clone( );
                id = p->get_state( ).add_registry_value( stub_number( ), cl );
            }
            return ast::node::make<ast_type>( pos, id );
        }

    private:
    };

    template <>
    class impl<type::TAIL_CALL>: public collectable<type::TAIL_CALL> {

        using this_type = impl<type::TAIL_CALL>;
    public:
        static const type type_value = type::TAIL_CALL;
        using sptr = std::shared_ptr<this_type>;

        impl(objects::sptr obj, objects::slist p, environment::sptr e)
            :collectable(e)
            ,obj_(obj)
            ,params_(std::move(p))
        { }

        ~impl( )
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "cc(" << ")";
            return oss.str( );
        }

        objects::sptr value( )
        {
            return obj_;
        }

        static
        sptr make( objects::sptr obj, objects::slist p, environment::sptr e )
        {
            return std::make_shared<this_type>( obj, std::move(p), e );
        }

        static
        sptr make( objects::sptr obj, environment::sptr e )
        {
            return std::make_shared<this_type>( obj, objects::slist { }, e );
        }

        objects::slist &params( )
        {
            return params_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( obj_, params_, env( ) );
        }

        ast::node::uptr to_ast( tokens::position /*pos*/ ) const override
        {
            return nullptr;
        }

    private:
        objects::sptr   obj_;
        objects::slist  params_;
    };

}}

#endif // FUNCTIONS_H
