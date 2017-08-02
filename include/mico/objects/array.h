#ifndef MICO_OBJECTS_ARRAY_H
#define MICO_OBJECTS_ARRAY_H

#include <deque>
#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"
#include "mico/objects/collectable.h"
#include "mico/expressions/array.h"

namespace mico { namespace objects {

    template <>
    class impl<type::ARRAY>: public collectable<type::ARRAY> {
        using this_type = impl<type::ARRAY>;
    public:

        static const type type_value = type::ARRAY;

        using sptr       = std::shared_ptr<this_type>;
        using cont       = impl<type::REFERENCE>;
        using cont_sptr  = std::shared_ptr<cont>;
        using value_type = std::deque<cont_sptr>;

        impl( environment::sptr env )
            :collectable<type::ARRAY>(env)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "[";
            bool first = true;
            for( auto &v: value( ) ) {
                if( first ) {
                    first = false;
                } else {
                    oss << ", ";
                }
                oss << v->value( )->str( );
            }
            oss << "]";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        std::size_t size( ) const override
        {
            return value_.size( );
        }

        value_type &value( )
        {
            return value_;
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

        cont_sptr at( std::int64_t id )
        {
            auto siz = fix_id( id );

            if( siz < value_.size( ) ) {
                return value_[siz];
            } else {
                return nullptr;
            }
        }

        void push( const environment * /*menv*/, objects::sptr val )
        {
            // mark in MY environment
            value_.emplace_back( cont::make_var( env( ).get( ), val) );
        }

        static
        sptr make( environment::sptr env )
        {
            return std::make_shared<this_type>( env );
        }

        hash_type hash( ) const override
        {
            auto init = static_cast<std::uint64_t>(get_type( ));
            std::uint64_t h = base::hash64( init );
            for( auto &o: value( ) ) {
                h = base::hash64( h + o->hash( ) );
            }
            return h;
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                if( o->value( ).size( ) == value( ).size( ) ) {
                    std::size_t id = value( ).size( );
                    while( id-- ) {
                        auto other = o->value( )[id]->value( ).get( );
                        if( !value( )[id]->equal( other ) ) {
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        }

        objects::sptr clone( ) const override
        {
            auto res = make( env( ) );
            for( auto &v: value_ ) {
                auto clone = v->value( )->clone( );
                res->push( v->env( ), clone );
            }
            return res;
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::ARRAY>;
            auto res = ast::node::make<ast_type>(pos);
            for( auto &v: value_ ) {
                auto next = v->value( )->to_ast( pos );
                res->value( ).emplace_back( ast::expression::cast( next ) );
            }
            return ast::node::uptr( std::move( res ) );
        }

    private:
        value_type  value_;
    };

    using array = impl<type::ARRAY>;
}}

#endif // ARRAY_H
