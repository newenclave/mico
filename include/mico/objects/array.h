#ifndef MICO_OBJECTS_ARRAY_H
#define MICO_OBJECTS_ARRAY_H

#include <deque>
#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"
#include "mico/objects/collectable.h"
#include "mico/expressions.h"

namespace mico { namespace objects {

    template <>
    class derived<type::ARRAY>: public collectable<type::ARRAY> {
        using this_type = derived<type::ARRAY>;
    public:

        static const type type_value = type::ARRAY;

        using sptr = std::shared_ptr<this_type>;
        using cont = derived<type::REFERENCE>;
        using cont_sptr = std::shared_ptr<cont>;
        using value_type = std::deque<cont_sptr>;
        using ast_type   = ast::expressions::array;

        derived( environment::sptr env )
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

        objects::sptr at( std::int64_t id )
        {
            auto size = static_cast<std::size_t>(id);
            if( (size < value_.size( )) && ( id >= 0) ) {
                return value_[size];
            } else {
                return derived<type::NULL_OBJ>::make( );
            }
        }

        void push( const environment *env, objects::sptr val )
        {
            value_.emplace_back( cont::make(env, val) );
        }

        static
        sptr make( environment::sptr env )
        {
            return std::make_shared<this_type>( env );
        }

        std::uint64_t hash( ) const override
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

        std::shared_ptr<base> clone( ) const override
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
            auto res = ast_type::uptr( new ast_type );
            res->set_pos( pos );
            for( auto &v: value( ) ) {
                auto next = v->value( )->to_ast( pos );
                auto r = ast::expression::cast(next);
                res->value( ).emplace_back( std::move(r) );
            }
            return res;
        }

    private:
        value_type  value_;
    };

}}

#endif // ARRAY_H
