#ifndef MICO_OBJECTS_TABLE_H
#define MICO_OBJECTS_TABLE_H

#include <unordered_map>
#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"

namespace mico { namespace objects {

    struct hash_helper {

        std::size_t operator ( )(const objects::sptr &h) const
        {
            return static_cast<std::size_t>(h->hash( ));
        }
    };

    struct equal_helper {

        bool operator ( )( const objects::sptr &l,
                           const objects::sptr &r ) const
        {
            return l->equal( r.get( ) );
        }
    };

    template <>
    class derived<type::TABLE>: public typed_base<type::TABLE> {

        using this_type = derived<type::TABLE>;
    public:
        using sptr = std::shared_ptr<this_type>;
        using cont = derived<type::REFERENCE>;
        using cont_sptr = std::shared_ptr<cont>;

        using value_type = std::unordered_map<objects::sptr, cont_sptr,
                                              hash_helper, equal_helper>;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "{ ";
            for( auto &v: value_ ) {
                oss << v.first->str( ) << ":";
                oss << v.second->value( )->str( )<< " ";
            }
            oss << "}";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        bool insert( const environment *env,
                     objects::sptr key, objects::sptr val )
        {
            value_[key->clone( )] = cont::make(env, val);
            return true;
        }

        std::uint64_t hash( ) const override
        {
            auto h = static_cast<std::uint64_t>(get_type( ));
            for( auto &o: value( ) ) {
                h = base::hash64( h + o.first->hash( ) +
                                     o.second->hash( ) );
            }
            return h;
        }

        objects::sptr at( objects::sptr id )
        {
            objects::sptr ptr = id;
            auto f = value_.find( ptr );
            if(f == value_.end( )) {
                return derived<type::NULL_OBJ>::make( );
            } else {
                return f->second;
            }
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                if( o->value( ).size( ) == value( ).size( ) ) {
                    for( auto &next: value( ) ) {
                        auto f = o->value( ).find( next.first );
                        if( f == o->value( ).end( ) ) {
                            return false;
                        }
                        auto ch = f->second->value( );
                        if( !next.second->value( )->equal( ch.get( ) ) ) {
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        }

        static
        sptr make( )
        {
            return std::make_shared<this_type>( );
        }


        bool lock_in( const environment *e ) override
        {
            for( auto &d: value_ ) {
                d.second->lock_in( e );
            }
            return true;
        }

        bool unlock_in( const environment *e ) override
        {
            for( auto &d: value_ ) {
                d.second->unlock_in( e );
            }
            return true;
        }

        std::size_t locked( ) const override
        {
            return 0;
        }

        std::shared_ptr<base> clone( ) const override
        {
            using ref = derived<type::REFERENCE>;
            auto res = make( );
            for( auto &v: value( ) ) {
                auto kc = v.first->clone( );
                auto vc = ref::make( v.second->env( ),
                                     v.second->value( )->clone( ) );
                res->value( ).insert( std::make_pair(kc, vc) );
            }
            return res;
        }

    private:

        void env_reset( ) override
        {
            for( auto &v: value_ ) {
                v.second->value( ).reset( );
                v.second.reset( );
            }
        }
        value_type value_;
    };

}}

#endif // TABLE_H
