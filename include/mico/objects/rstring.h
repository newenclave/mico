#ifndef MICO_OBJECTS_RSTRING_H
#define MICO_OBJECTS_RSTRING_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/objects/character.h"
#include "mico/objects/numbers.h"
#include "mico/expressions/string.h"

#include "mico/types.h"

namespace mico { namespace objects {

    template <>
    class impl<type::RSTRING>: public typed_base<type::RSTRING> {

        using this_type = impl<type::RSTRING>;

    public:

        using internal_type = std::string;
        using system_type   = std::string;

        static const type type_value = type::RSTRING;

        using sptr        = std::shared_ptr<this_type>;
        using value_type  = internal_type;
        using symbol_type = std::uint8_t;

        using slice_type = impl<type::RSLICE>;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "r\"" << value( ) << "\"";
            return oss.str( );
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
            auto seed = static_cast<std::size_t>(get_type( ));
            for( auto &a: value_ ) {
                seed = hash_combine( seed, a );
            }
            return seed;
        }

        std::size_t fix_id( std::int64_t id ) const
        {
            auto siz = static_cast<std::size_t>(id);
            if( id < 0 ) {
                siz = size( ) - static_cast<std::size_t>(-1 * id);
            }
            return siz;
        }

        bool valid_id( std::int64_t id ) const
        {
            return fix_id(id) < value_.size( );
        }

        std::size_t size( ) const override
        {
            return value_.size( );
        }

        symbol_type at( std::int64_t id )
        {
            auto siz = fix_id( id );
            if( siz < value_.size( ) ) {
                return static_cast<std::uint8_t>(value_[siz]);
            } else {
                return 0;
            }
        }

        objects::sptr at_obj( std::int64_t id )
        {
            return objects::integer::make( at(id) );
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<this_type>( std::move(val) );
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
            using ast_type = ast::expressions::impl<ast::type::STRING>;
            return ast::node::make<ast_type>( pos, value_, true );
        }

    private:

        value_type value_;

    };

    using rstring = impl<type::RSTRING>;

}}

#endif // STRING_H
