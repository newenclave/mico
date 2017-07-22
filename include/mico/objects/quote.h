#ifndef MICO_OBJECTS_QUOTE_H
#define MICO_OBJECTS_QUOTE_H

#if !defined(DISABLE_MACRO)

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/string.h"

namespace mico { namespace objects {

    template <>
    class impl<type::QUOTE>: public typed_base<type::QUOTE> {

        using this_type = impl<type::QUOTE>;

    public:

        static const type type_value = type::QUOTE;

        using sptr = std::shared_ptr<this_type>;
        using value_type = ast::node::sptr;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "quote(" << value( )->str( ) << ")";
            return oss.str( );
        }

        impl( ast::node::uptr val )
            :value_(val.release( ))
        { }

        impl( ast::node::sptr val )
            :value_(val)
        { }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        static
        sptr make( ast::node::uptr val )
        {
            return std::make_shared<this_type>( std::move(val) );
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<this_type>( value_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::QUOTE>;
            return ast::node::make<ast_type>( pos, value( )->clone( ) );
        }

    private:
        value_type value_;

    };

}}

#endif // !defined(DISABLE_MACRO)
#endif // STRING_H
