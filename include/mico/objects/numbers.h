#ifndef MICO_OBJECTS_NUMBERS_H
#define MICO_OBJECTS_NUMBERS_H

#include <sstream>
#include "mico/objects/base.h"
#include "mico/objects/boolean.h"
#include "mico/expressions/value.h"

namespace mico { namespace objects {

    template <type TName>
    struct type2object;

    template <>
    struct type2object<type::FLOAT> {
        using native_type = double;
        using ast_type    = ast::expressions::impl<ast::type::FLOAT>;
    };

    template <>
    struct type2object<type::INTEGER> {
        using native_type = std::int64_t;
        using ast_type    = ast::expressions::impl<ast::type::INTEGER>;
    };

    template <type TN>
    class impl: public typed_base<TN>  {
        using this_type = impl<TN>;
    public:

        static const type type_value = TN;
        using sptr = std::shared_ptr<this_type>;

        using value_type = typename type2object<TN>::native_type;

        explicit
        impl(value_type val)
            :value_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << value( );
            return oss.str( );
        }

        value_type value( ) const
        {
            return value_;
        }

        void set_value( value_type val )
        {
            value_ = val;
        }

        template <typename T>
        static
        sptr make( T val )
        {
            return std::make_shared<this_type>( static_cast<value_type>(val) );
        }

        static
        std::size_t hash(value_type x )
        {
            std::hash<value_type> h;
            return h(x);
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == this->get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return o->value( ) == value( );
            }
            return false;
        }

        objects::sptr clone( ) const override
        {
            return make( value_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = typename type2object<TN>::ast_type;
            return ast::node::make<ast_type>(pos, value_);
        }

    private:

        value_type value_;
    };

    using integer  = impl<type::INTEGER>;
    using floating = impl<type::FLOAT>;

    struct numeric {

        static
        bool is_numeric( objects::sptr obj )
        {
            return ( obj->get_type( ) == type::FLOAT   )
                || ( obj->get_type( ) == type::INTEGER )
                 ;
        }

        static
        bool should_float( objects::sptr obj1, objects::sptr obj2 )
        {
            return is_numeric( obj1 ) && is_numeric( obj2 ) &&
                  ( (obj1->get_type( ) == type::FLOAT) ||
                    (obj2->get_type( ) == type::FLOAT) )
                 ;
        }

        static
        objects::floating::sptr to_float( objects::sptr obj )
        {
            if( obj->get_type( ) == type::FLOAT ) {
                return objects::cast<type::FLOAT>(obj);
            } else if( obj->get_type( ) == type::INTEGER ) {
                auto in = objects::cast<type::INTEGER>( obj );
                auto val = static_cast<double>(in->value( ));
                return objects::floating::make(val);
            } else if( obj->get_type( ) == type::BOOLEAN ) {
                auto in = objects::cast<type::BOOLEAN>( obj );
                auto val = in->value( ) ? 1.0 : 0.0;
                return objects::floating::make(val);
            }
            return nullptr;
        }

        static
        objects::integer::sptr to_int( objects::sptr obj )
        {
            if( obj->get_type( ) == type::INTEGER ) {
                return objects::cast<type::INTEGER>(obj);

            } else if( obj->get_type( ) == type::FLOAT ) {
                auto in = objects::cast<type::FLOAT>( obj );
                auto val = static_cast<std::int64_t>(in->value( ));
                return objects::integer::make(val);

            } else if( obj->get_type( ) == type::BOOLEAN ) {
                auto in = objects::cast<type::BOOLEAN>( obj );
                std::int64_t val = in->value( ) ? 1 : 0;
                return objects::integer::make(val);
            }
            return nullptr;
        }

        static
        objects::boolean::sptr to_bool( objects::sptr obj )
        {
            if( obj->get_type( ) == type::BOOLEAN ) {
                return objects::cast<type::BOOLEAN>(obj);

            } else if( obj->get_type( ) == type::FLOAT ) {
                auto in = objects::cast<type::FLOAT>( obj );
                auto val = in->value( );
                return objects::boolean::make( (val > 0) || (val < 0) );

            } else if( obj->get_type( ) == type::INTEGER ) {
                auto in = objects::cast<type::INTEGER>( obj );
                std::int64_t val = in->value( );
                return objects::boolean::make(!!val);

            }
            return nullptr;
        }
    };

}}

#endif // NUMBERS_H
