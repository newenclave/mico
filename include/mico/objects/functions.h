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

        derived( std::shared_ptr<environment> e,
                  std::shared_ptr<ast::expression_list> par,
                  std::shared_ptr<ast::statement_list> st )
            :collectable(e)
            ,params_(par)
            ,body_(st)
        { }

        ~derived( )
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(" << params_->size( ) << ")";
            return oss.str( );
        }

        ast::expression_list &params( )
        {
            return *params_;
        }

        const ast::expression_list &params( ) const
        {
            return *params_;
        }

        const ast::statement_list &body( ) const
        {
            return *body_;
        }

        ast::statement_list &body( )
        {
            return *body_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( env( ), params_, body_ );
        }

    private:

        std::shared_ptr<ast::expression_list> params_;
        std::shared_ptr<ast::statement_list>  body_;
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
