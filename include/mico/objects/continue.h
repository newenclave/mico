#ifndef MICO_OBJECTS_CONTINUE_H
#define MICO_OBJECTS_CONTINUE_H

#include "mico/objects/base.h"
#include "mico/statements.h"

namespace mico { namespace objects {
    template <>
    class impl<type::CONT_OBJ>: public typed_base<type::CONT_OBJ> {
        using this_type = impl<type::CONT_OBJ>;
    public:

        static const type type_value = type::CONT_OBJ;

        using sptr = std::shared_ptr<this_type>;

        std::string str( ) const override
        {
            return "continue";
        }

        static
        sptr make( )
        {
            static auto val = std::make_shared<this_type>( );
            return val;
        }

        hash_type hash( ) const override
        {
            return 0;
        }

        bool equal( const base *o ) const override
        {
            return (o->get_type( ) == get_type( ));
        }

        objects::sptr clone( ) const override
        {
            return make( );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            auto res = ast::node::make<ast::statements::cont_expr>( pos );
            return ast::node::uptr( std::move( res ) );
        }

    };

    using cont_obj = impl<type::CONT_OBJ>;

}}

#endif // NULL_H
