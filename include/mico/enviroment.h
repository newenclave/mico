#ifndef MICO_ENVIROMENT_H
#define MICO_ENVIROMENT_H

#include <set>
#include <map>
#include <memory>

//#include "mico/objects.h"

namespace mico {

namespace objects {
    struct base;
}

    class enviroment {

    public:

        using sptr = std::shared_ptr<enviroment>;
        using wptr = std::weak_ptr<enviroment>;
        using object_sptr = std::shared_ptr<objects::base>;

    protected:

        struct key { };

    public:

        enviroment( key ) { }
        enviroment( sptr env, key )
            :parent_(env)
        { }

        static
        sptr make( )
        {
            return std::make_shared<enviroment>( key( ) );
        }

        static
        sptr make( sptr parent )
        {
            auto res = std::make_shared<enviroment>( parent, key( ) );
            parent->children_[res.get( )] = res;
            return res;
        }

        void drop( enviroment *child )
        {
            children_.erase( child );
            if( children_.empty( ) ) {
                auto p = parent_.lock( );
                if( p ) {
                    p->drop( this );
                }
            }
        }

        void drop( )
        {
            if( children_.empty( ) ) {
                auto p = parent_.lock( );
                if( p ) {
                    p->drop( this );
                }
            }
        }

        void clear(  )
        {
            parent_.reset( );
            data_.clear( );
        }

        void set( const std::string &name, object_sptr val )
        {
            data_[name] = val;
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
                    parent = cur->parent_.lock( );
                    cur = parent.get( );
                }
            }
            return res;
        }

    private:

        wptr parent_;
        std::map<std::string, object_sptr> data_;
        std::map<enviroment *, sptr> children_;
    };
}

#endif // ENVIROMENT_H
