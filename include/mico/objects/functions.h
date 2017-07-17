#ifndef MICO_OBJECTS_FUNCTIONS_H
#define MICO_OBJECTS_FUNCTIONS_H

#include "mico/objects/reference.h"
#include "mico/objects/collectable.h"

namespace mico { namespace objects {

    template <>
    class derived<type::FUNCTION>: public collectable<type::FUNCTION> {
        using this_type = derived<type::FUNCTION>;
    public:
        static const type type_value = type::FUNCTION;
        using sptr = std::shared_ptr<this_type>;

        using param_iterator = ast::expression_list::iterator;

        derived( std::shared_ptr<environment> e,
                 std::shared_ptr<ast::expression_list> par,
                 std::shared_ptr<ast::statement_list> st,
                 std::size_t start = 0)
            :collectable(e)
            ,params_(par)
            ,body_(st)
            ,start_param_(start)
        { }

        ~derived( )
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            if( start_param_ == 0  ) {
                oss << "fn(" << param_size( ) << ")";
            } else {
                oss << "fn(" << param_size( ) << ":" << params_->size( ) << ")";
            }
            return oss.str( );
        }

        const ast::statement_list &body( ) const
        {
            return *body_;
        }

        static
        sptr make( std::shared_ptr<environment> e,
                   std::shared_ptr<ast::expression_list> par,
                   std::shared_ptr<ast::statement_list> st,
                   std::size_t start = 0 )
        {
            return std::make_shared<derived>( e, par, st, start );
        }

        static
        sptr make( std::shared_ptr<environment> e,
                   this_type &other, std::size_t start )
        {
            return std::make_shared<derived>( e, other.params_,
                                              other.body_,
                                              start + other.start_param_ );
        }

        static
        sptr make_from_partial( this_type::sptr other )
        {
            if( other->start_param_ != 0 ) {
                if( auto p = other->env( ) ) {
                    return std::make_shared<derived>( p->parent( ),
                                                      other->params_,
                                                      other->body_, 0 );
                }
            }
            return other;
        }

        param_iterator begin( )
        {
            return params_->begin( ) + start_param_;
        }

        param_iterator end( )
        {
            return params_->end( );
        }

        std::size_t param_size( ) const
        {
            return params_->size( ) - start_param_;
        }

        ast::statement_list &body( )
        {
            return *body_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( env( ), params_, body_,
                                                start_param_ );
        }

    private:

        std::shared_ptr<ast::expression_list> params_;
        std::shared_ptr<ast::statement_list>  body_;
        std::size_t start_param_ = 0;
    };

    template <>
    class derived<type::BUILTIN>: public collectable<type::BUILTIN> {
        using this_type = derived<type::BUILTIN>;
    public:
        static const type type_value = type::BUILTIN;

        using sptr = std::shared_ptr<this_type>;

        derived( std::shared_ptr<environment> e )
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
            return derived<type::NULL_OBJ>::make( );
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
        void init( environment::sptr )
        { }

    private:
    };

    template <>
    class derived<type::TAIL_CALL>: public collectable<type::TAIL_CALL> {

        using this_type = derived<type::TAIL_CALL>;
    public:
        static const type type_value = type::TAIL_CALL;
        using sptr = std::shared_ptr<this_type>;

        derived(objects::sptr obj, objects::slist p, environment::sptr e)
            :collectable(e)
            ,obj_(obj)
            ,params_(std::move(p))
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

    private:
        objects::sptr   obj_;
        objects::slist  params_;
    };

}}

#endif // FUNCTIONS_H
