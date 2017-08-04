#ifndef MICO_OBJECTS_INFINITE_H
#define MICO_OBJECTS_INFINITE_H

#include "mico/objects/base.h"
#include "mico/expressions/infinite.h"

namespace mico { namespace objects {
    template <>
    class impl<type::INF_OBJ>: public typed_base<type::INF_OBJ> {
        using this_type = impl<type::INF_OBJ>;
    public:

        static const type type_value = type::INF_OBJ;
        using sptr = std::shared_ptr<this_type>;

        impl<type::INF_OBJ>( bool negative )
            :negative_(negative)
        {

        }

        std::string str( ) const override
        {
            return negative_ ? "-inf" : "inf";
        }

        bool is_negative( ) const
        {
            return negative_;
        }

        static
        sptr make( bool negative )
        {
            static auto neg = std::make_shared<this_type>(true);
            static auto pos = std::make_shared<this_type>(false);
            return negative ? neg : pos;
        }

        hash_type hash( ) const override
        {
            return static_cast<hash_type>(get_type( ));
        }

        bool equal( const base *o ) const override
        {
            static auto pos = make(false);
            static auto neg = make(true);
            return negative_ ? o == neg.get( ) : o == pos.get( );
        }

        objects::sptr clone( ) const override
        {
            return make(negative_);
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            return ast::node::make<ast::expressions::infinite>(pos, negative_);
        }

    private:

        bool negative_ = false;

    };

    using infinite = impl<type::INF_OBJ>;

}}

#endif // NULL_H
