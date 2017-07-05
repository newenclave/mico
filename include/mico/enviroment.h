#ifndef MICO_ENVIROMENT_H
#define MICO_ENVIROMENT_H

#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

//#include "mico/objects.h"

namespace mico {

namespace objects {
    struct base;
}

#ifdef DEBUG
    static int c = 0;
#endif

    class enviroment: public std::enable_shared_from_this<enviroment> {

    public:

        using sptr = std::shared_ptr<enviroment>;
        using wptr = std::weak_ptr<enviroment>;
        using object_sptr = std::shared_ptr<objects::base>;
        using object_wptr = std::weak_ptr<objects::base>;
        using children_type = std::map<enviroment *, wptr>;

    protected:

        struct key { };

    public:

        struct scoped {

            scoped( sptr v )
                :env_(v)
            {
                env_->lock( );
            }

            ~scoped(  )
            {
                if( env_ ) {
                    env_->unlock( );
                    env_->clear( );
                    env_->drop( );
                }
            }

            sptr env( )
            {
                return env_;
            }

        private:
            sptr env_;
        };

        enviroment( key )
        {
#ifdef DEBUG
            std::cout << ++c << "\n";
#endif
        }

        enviroment( sptr env, key )
            :parent_(env)
        {
#ifdef DEBUG
            std::cout << ++c << "\n";
#endif
        }

        ~enviroment( )
        {
#ifdef DEBUG
            std::cout << --c << "\n";
#endif
        }

        void clear( )
        {
            if( children_.empty( ) ) {
                data_.clear( );
            }
        }

        static
        sptr make( )
        {
            return std::make_shared<enviroment>( key( ) );
        }

        static
        sptr make( sptr parent )
        {
            auto res = std::make_shared<enviroment>( parent, key( ) );
            parent->children_ [res.get( )] = res;
            return res;
        }

        void drop( enviroment * child )
        {
            children_.erase( child );
            drop( );
        }

        void drop( )
        {
            if( children_.empty( ) && !locked_ ) {
                auto p = parent_;
                if( p ) {
                    //data_.clear( );
                    p->drop( this );
                }
            }
        }

        void lock( )
        {
            locked_++;
        }

        std::size_t locked( ) const
        {
            return locked_;
        }

        void unlock( )
        {
            locked_--;
        }

        sptr find_contains( const std::string &name )
        {
            auto cur = this;
            sptr parent = shared_from_this( );
            while( cur ) {
                auto f = cur->data_.find( name );
                if( f != cur->data_.end( ) ) {
                    return parent;
                } else {
                    parent = cur->parent_;
                    cur = parent.get( );
                }
            }
            return nullptr;
        }

        void set( const std::string &name, object_sptr val )
        {
            data_[name] = val;
            keeped_[val.get( )] = val;
        }

        void add_keep( void *key, object_sptr val )
        {
            keeped_[key] = val;
        }

        void del_keep( void *key )
        {
            keeped_.erase(key);
        }

        sptr parent( )
        {
            return parent_;
        }

        object_sptr get( const std::string &name )
        {
            auto cur = this;
            object_sptr res;
            sptr parent;
            while( cur && !res ) {
                auto f = cur->data_.find( name );
                if( f != cur->data_.end( ) ) {
                    res = f->second;
                } else {
                    parent = cur->parent( );
                    cur = parent.get( );
                }
            }
            return res;
        }

        std::map<std::string, object_sptr> &data( )
        {
            return data_;
        }

        children_type &children( )
        {
            return children_;
        }

        void introspect( int level = 0 )
        {
            std::string space( level * 2, ' ' );
            std::cout << space << locked( ) << "\n";
            for( auto &d: data_ ) {
                std::weak_ptr<objects::base> w = d.second;
                auto us = d.second.use_count( );
                std::cout << space << d.first << ":"
                          << us
                          << "\n";
            }

            for( auto &c: children_ ) {
                auto cl = c.second.lock( );
                if( !cl ) continue;
                auto us = cl.use_count( );
                std::cout << space << "Child: "
                          << us
                          << " l: " << cl->locked_
                          << "\n";
                cl->introspect( level + 1 );
            }
        }

        std::size_t maximum_level( )
        {
            std::size_t res = 1;
            maximum_level_hide( res );
            return res;
        }

        void GC( )
        {
            for( auto &c: children( ) ) {
                auto cl = c.second.lock( );
                //if( c->maximum_level( ) == 1  ) {
                    cl->unlock( );
                    cl->children( ).clear( );
                    cl->data( ).clear( );
                    //cl->clear( );
                    //c->drop( );
                //}
            }
        }

    private:

        void maximum_level_hide( std::size_t &res )
        {
            for( auto &d: data_ ) {
                auto us = static_cast<std::size_t>(d.second.use_count( ));
                res = (res > us) ? res : us;
            }
            for( auto &c: children_ ) {
                auto cl = c.second.lock( );
                if( !cl ) {
                    continue;
                }
                auto us = static_cast<std::size_t>(cl.use_count( ));
                res = (res > us) ? res : us;
                cl->maximum_level_hide( res );
            }
        }

        sptr parent_;
        std::map<std::string, object_sptr> data_;
        children_type children_;
        std::map<void *, object_wptr> keeped_;
        std::size_t locked_ = 0;
    };
}

#endif // ENVIROMENT_H
