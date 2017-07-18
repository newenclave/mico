#ifndef MICO_OBJECTS_QUOTE_H
#define MICO_OBJECTS_QUOTE_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/quote.h"

namespace mico { namespace objects {

    template <>
    class derived<type::QUOTE>: public typed_base<type::QUOTE> {

        using this_type = derived<type::QUOTE>;

    public:

        static const type type_value = type::QUOTE;

        using sptr       = std::shared_ptr<this_type>;
        using ast_type   = ast::expressions::detail<ast::type::QUOTE>;
        using value_type = ast_type::value_type;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "`(" << value( )->str( ) << ")";
            return oss.str( );
        }

        derived( value_type val )
            :value_(val)
        { }

        value_type value( ) const
        {
            return value_;
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<this_type>( val );
        }

        std::shared_ptr<base> clone( ) const override
        {
            return make( value_ );
        }

        ast::node::uptr to_ast( tokens::position ) const override
        {
            return value_->clone( );
        }

    private:

        value_type value_;
    };

}}

#endif // QUOTE_H
