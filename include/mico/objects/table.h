#ifndef MICO_OBJECTS_TABLE_H
#define MICO_OBJECTS_TABLE_H

#include <unordered_map>
#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"
#include "mico/objects/collectable.h"
#include "mico/expressions/table.h"

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
    class impl<type::TABLE>: public collectable<type::TABLE> {

        using this_type = impl<type::TABLE>;
    public:

        static const type type_value = type::TABLE;
        using sptr = std::shared_ptr<this_type>;
        using cont = impl<type::REFERENCE>;
        using cont_sptr = std::shared_ptr<cont>;

        using value_type = std::unordered_map<objects::sptr, cont_sptr,
                                              hash_helper, equal_helper>;

        impl( environment::sptr e )
            :collectable<type::TABLE>(e)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "{ ";
            bool first = true;
            for( auto &v: value_ ) {
                if( !first ) {
                    oss << ", ";
                } else {
                    first = false;
                }
                oss << v.first->str( ) << ":";
                oss << v.second->value( )->str( );
            }
            oss << " }";
            return oss.str( );
        }

        std::size_t size( ) const override
        {
            return value_.size( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        bool set( const environment *env, objects::sptr key, objects::sptr val )
        {
            value_[key->clone( )] = cont::make_var(env, val);
            return true;
        }

        bool insert( const environment *env,
                     objects::sptr key, objects::sptr val )
        {
            auto f = value_.find( key );
            if( f == value_.end( ) ) {
                return set( env, key, val );
            }
            return false;
        }

        hash_type hash( ) const override
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
                return nullptr;
            } else {
                return is_mutable( ) ? f->second : f->second->value( );
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
        sptr make( environment::sptr env )
        {
            return std::make_shared<this_type>( env );
        }

        std::size_t marked( ) const override
        {
            return 0;
        }

        objects::sptr clone( ) const override
        {
            using ref = impl<type::REFERENCE>;
            auto res = make( env( ) );
            for( auto &v: value( ) ) {
                auto kc = v.first->clone( );
                auto vc = ref::make_var( v.second->env( ),
                                         v.second->value( )->clone( ) );
                res->value( ).insert( std::make_pair(kc, vc) );
            }
            res->set_mutable( is_mutable( ) );
            return res;
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::TABLE>;
            ast_type::uptr res(new ast_type);
            res->set_pos(pos);
            for( auto &v: value_ ) {
                auto fast = v.first->to_ast( pos );
                auto sast = v.second->value( )->to_ast( pos );
                res->value( ).emplace_back( ast::expression::cast(fast),
                                            ast::expression::cast(sast) );
            }
            if( is_mutable( ) ) {
                auto mut = ast::expressions::mod_mut::make(std::move( res ));
                return ast::node::uptr( std::move( mut ) );
            } else {
                return ast::node::uptr( std::move( res ) );
            }
        }

    private:
        value_type value_;
    };

    using table = impl<type::TABLE>;

}}

#endif // TABLE_H
