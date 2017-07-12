#ifndef MICO_OBJECTS_ARRAY_H
#define MICO_OBJECTS_ARRAY_H

#include <deque>
#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"

namespace mico { namespace objects {

    template <>
    class derived<type::ARRAY>: public typed_base<type::ARRAY> {
        using this_type = derived<type::ARRAY>;
    public:

        using sptr = std::shared_ptr<this_type>;
        using cont = derived<type::REFERENCE>;
        using cont_sptr = std::shared_ptr<cont>;
        using value_type = std::deque<cont_sptr>;

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
            val->lock_in( env );
        }

        static
        sptr make( )
        {
            return std::make_shared<this_type>( );
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

        bool lock_in( const environment *e ) override
        {
            for( auto &d: value_ ) {
                d->lock_in( e );
            }
            return true;
        }

        bool unlock_in( const environment *e ) override
        {
            for( auto &d: value_ ) {
                d->unlock_in( e );
            }
            return true;
        }

        std::shared_ptr<base> clone( ) const override
        {
            auto res = std::make_shared<this_type>( );
            for( auto &v: value_ ) {
                auto clone = v->value( )->clone( );
                clone->lock_in( v->cont_env( ) );
                res->push( v->env( ), clone );
            }
            return res;
        }

    private:

        void env_reset( ) override
        {
            for( auto &v: value_ ) {
                //v->value( ).reset( );
                v.reset( );
            }
        }
        value_type  value_;
    };

}}

#endif // ARRAY_H
