#ifndef MICO_OBJECT_REFERENCE_H
#define MICO_OBJECT_REFERENCE_H

#include "mico/objects/base.h"
#include "mico/environment.h"
#include "mico/statements.h"
#include "mico/expressions.h"

namespace mico { namespace objects {

    template <>
    class impl<type::REFERENCE>: public typed_base<type::REFERENCE> {
        using this_type = impl<type::REFERENCE>;
    public:

        static const type type_value = type::REFERENCE;

        using sptr = std::shared_ptr<this_type>;
        using value_type = objects::sptr;

        impl(const environment *my_env, value_type val)
            :my_env_(my_env)
            ,value_(unref(val))
        {
            value_->mark_in( my_env_ );
            marked_ = val->marked( );
        }

        ~impl( )
        {
            value_->unmark_in( my_env_ );
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "*"  << value( )->str( ) << "";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        static
        objects::sptr unref( objects::sptr obj )
        {
            if( obj->get_type( ) == objects::type::REFERENCE ) {
                auto ret = static_cast<this_type *>(obj.get( ));
                return ret->value( );
            } else {
                return obj;
            }
        }

        void set_value( const environment *my_env, value_type val )
        {
            if( value_ != val ) {
                //// unlock
                value_->unmark_in( my_env_ );

                ///replace lock
                value_ = val;
                my_env_ = my_env;
                value_->mark_in( my_env_ );
                marked_ = val->marked( );
            }
        }

        static
        sptr make( const environment *my_env, value_type val )
        {
            return std::make_shared<this_type>(my_env, val);
        }

        const environment *env( ) const
        {
            return my_env_;
        }

        bool mark_in( const environment *e ) override
        {
            return value_->mark_in( e );
        }

        bool unmark_in( const environment *e ) override
        {
            return value_->unmark_in( e );
        }

        std::uint64_t hash( ) const override
        {
            return value_->hash( );
        }

        bool equal( const base *other ) const override
        {
            return value_->equal( other );
        }

        std::shared_ptr<base> clone( ) const override
        {
            auto res = std::make_shared<this_type>( my_env_, value_->clone( ) );
            return res;
        }

        std::size_t marked( ) const override
        {
            return marked_;
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            return value_->to_ast( pos );
        }

    private:
        const environment  *my_env_;
        value_type          value_;
        std::size_t         marked_ = 0;
    };


}}

#endif // OBJECT_REFERENCE_H
