#ifndef MICO_OBJECTS_TYPE_H
#define MICO_OBJECTS_TYPE_H

#include "mico/objects/base.h"

namespace mico { namespace objects {

    template <>
    class impl<type::TYPE_OBJ>: public typed_base<type::TYPE_OBJ> {
        using this_type = impl<type::TYPE_OBJ>;
    public:

        static const type type_value = type::TYPE_OBJ;
        using sptr = std::shared_ptr<this_type>;

        explicit
        impl<type::TYPE_OBJ>( objects::type tt )
            :me_(tt)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "type(" << me_<< ")";
            return oss.str( );
        }

        objects::type value( ) const
        {
            return me_;
        }

        static
        sptr make( objects::type tt )
        {
            auto val = std::make_shared<this_type>( tt );
            return val;
        }

        hash_type hash( ) const override
        {
            hash_type seed = static_cast<hash_type>(get_type( ));
            return base::hash_combine( seed, static_cast<hash_type>(me_) );
        }

        bool equal( const base *o ) const override
        {
            if(o->get_type( ) == get_type( )) {
                auto other = objects::cast<objects::type::TYPE_OBJ>(o);
                return other->me_ == me_;
            }
            return false;
        }

        objects::sptr clone( ) const override
        {
            return make( me_ );
        }

        ast::node::uptr to_ast( tokens::position /*pos*/ ) const override
        {
            return nullptr;
        }

    private:
        objects::type me_;
    };

    using null = impl<type::NULL_OBJ>;

}}

#endif // NULL_H
