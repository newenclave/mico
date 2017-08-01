#ifndef MICO_OBJECTS_CHAR_H
#define MICO_OBJECTS_CHAR_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/string.h"

#include "mico/types.h"

namespace mico { namespace objects {

    template <>
    class impl<type::CHARACTER>: public typed_base<type::CHARACTER> {

        using this_type = impl<type::CHARACTER>;

    public:

        using internal_type = mico::string;
        using system_type   = mico::file_string;

        static const type type_value = type::STRING;

        using sptr        = std::shared_ptr<this_type>;
        using value_type  = internal_type::value_type;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "'" << to_file_str( ) << "'";
            return oss.str( );
        }

        std::string to_file_str(  ) const
        {
            mico::string v(1, value( ));
            return charset::encoding::to_file( v );
        }

        mico::string to_str(  ) const
        {
            return mico::string(1, value( ));
        }

        impl( value_type val )
            :value_(std::move(val))
        { }

        const value_type &value( ) const
        {
            return value_;
        }
        value_type &value( )
        {
            return value_;
        }

        hash_type hash( ) const override
        {
            std::hash<value_type> h;
            return h(value( ));
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<this_type>( val );
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return o->value( ) == value( );
            }
            return false;
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<this_type>( value_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::CHARACTER>;
            return ast::node::make<ast_type>( pos, value( ) );
        }

    private:

        value_type value_;

    };

    using character = impl<type::CHARACTER>;

}}

#endif // STRING_H
