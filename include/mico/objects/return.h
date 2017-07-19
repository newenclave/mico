#ifndef MICO_OBJECTS_RETURN_H
#define MICO_OBJECTS_RETURN_H

#include <sstream>
#include "mico/objects/base.h"
#include "mico/statements.h"

namespace mico { namespace objects {

    template <>
    class derived<type::RETURN>: public typed_base<type::RETURN> {
        using this_type = derived<type::RETURN>;
    public:
        static const type type_value = type::RETURN;

        using sptr = std::shared_ptr<this_type>;

        using value_type = objects::sptr;

        derived( value_type val )
            :value_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "return " << value( )->str( );
            return oss.str( );
        }

        value_type &value( )
        {
            return value_;
        }

        const value_type &value( ) const
        {
            return value_;
        }

        static
        sptr make( objects::sptr res )
        {
            return std::make_shared<this_type>( res );
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( value_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::statements::detail<ast::type::RETURN>;
            auto obj = value_->to_ast( pos );
            ast_type::uptr res( new ast_type( ast::expression::cast(obj) ) );
            res->set_pos(pos);
            return res;
        }

    private:
        value_type value_;
    };

}}

#endif // RETURN_H
