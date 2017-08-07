#ifndef MICO_OBJECTS_INTERVAL_H
#define MICO_OBJECTS_INTERVAL_H

#include <string>
#include <sstream>
#include <map>
#include "mico/objects/base.h"
#include "mico/expressions/none.h"
#include "mico/objects/array.h"
#include "mico/objects/string.h"
#include "mico/objects/numbers.h"

#include "etool/intervals/interval.h"

namespace mico { namespace objects {

    template<>
    class impl<type::INTERVAL>: public typed_base<type::INTERVAL> {

        using this_type = impl<type::INTERVAL>;

    public:

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << begin( )->str( ) << ".." << end( )->str( );
            return oss.str( );
        }

        impl<type::INTERVAL>( )
        { }

        ~impl<type::INTERVAL>( )
        { }

        virtual
        objects::sptr begin( ) const = 0;

        virtual
        objects::sptr end( ) const = 0;

        virtual
        objects::type domain( ) const = 0;

        virtual
        bool contains( const base *other ) const = 0;

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == this->get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return begin( )->equal( o->begin( ).get( ) )
                    && end( )->equal( o->end( ).get( ) );
            }
            return false;
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::infix;
            auto res = ast_type::make( tokens::type::DOTDOT,
                                       begin( )->to_ast( pos ) );
            res->set_right( end( )->to_ast( pos ) );

            return ast::node::uptr(std::move( res ) );
        }
    };

    using interval = impl<type::INTERVAL>;

    namespace intervals {

        template <objects::type NumT>
        struct obj: public interval {

            using this_type = obj<NumT>;

        public:

            static const objects::type type_name = NumT;

            using object_type   = objects::impl<type_name>;
            using value_type    = typename object_type::value_type;

            using sptr          = std::shared_ptr<this_type>;
            using interval_type = etool::intervals::interval<value_type>;

            obj<NumT>( value_type left, value_type right )
                :ival_(interval_type::closed(left, right))
            { }

            objects::sptr begin( ) const override
            {
                return object_type::make( ival_.left( ) );
            }

            objects::sptr end( ) const override
            {
                return object_type::make( ival_.right( ) );
            }

            objects::type domain( ) const override
            {
                return type_name;
            }

            static
            sptr make( value_type left, value_type right )
            {
                return std::make_shared<this_type>(left, right);
            }

            objects::sptr clone( ) const override
            {
                return make( ival_.left( ), ival_.right( ) );
            }

            bool contains( const base *other ) const override
            {
                if( other->get_type( ) == type_name ) {
                    auto val = objects::cast<type_name>(other);
                    return native( ).contains( val->value( ) );
                }
                return false;
            }

            std::size_t hash( ) const override
            {
                std::size_t seed = 0;
                hash_combine( seed, ival_.left( ) );
                hash_combine( seed, ival_.right( ));
                return seed;
            }

            interval_type &native( )
            {
                return ival_;
            }

            const interval_type &native( ) const
            {
                return ival_;
            }

        private:

            interval_type ival_;
        };

        using rstring   = obj<objects::type::RSTRING>;
        using string    = obj<objects::type::STRING>;
        using integer   = obj<objects::type::INTEGER>;
        using floating  = obj<objects::type::FLOAT>;
        using boolean   = obj<objects::type::BOOLEAN>;
    }

}}

#endif // INTERVAL_H
