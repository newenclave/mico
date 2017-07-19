#ifndef MICO_OBJECTS_NUMBERS_H
#define MICO_OBJECTS_NUMBERS_H

#include <sstream>
#include "mico/objects/base.h"
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

        static const type type_name = TN;
        using value_type = typename type2object<type_name>::native_type;

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

        std::shared_ptr<base> clone( ) const override
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

}}

#endif // NUMBERS_H
