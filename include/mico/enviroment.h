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

    static auto cout = 0;

    class enviroment: public std::enable_shared_from_this<enviroment> {

    public:

        using sptr = std::shared_ptr<enviroment>;
        using wptr = std::weak_ptr<enviroment>;
        using object_sptr = std::shared_ptr<objects::base>;

    protected:

        struct key { };

    public:

        struct scoped {

            scoped( sptr v )
                :env_(v)
            { }

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
        {  }

        enviroment( sptr env, key )
            :parent_(env)
        { }

        ~enviroment( )
        {
            //std::cout << --cout << "\n";
        }

        void clear( )
        {
            data_.clear( );
        }

        static
        sptr make( )
        {
            //std::cout << ++cout << "\n";
            return std::make_shared<enviroment>( key( ) );
        }

        static
        sptr make( sptr parent )
        {
            //std::cout << ++cout << "\n";
            auto res = std::make_shared<enviroment>( parent, key( ) );
            parent->children_.insert( res );
            return res;
        }

        void drop( sptr child )
        {
            children_.erase( child );
            drop( );
        }

        void drop( )
        {

            if( children_.empty( ) && !locked_ ) {
                auto p = parent_.lock( );
                if( p ) {
                    //data_.clear( );
                    p->drop( shared_from_this( ) );
                }
            }
        }

        void lock( )
        {
            locked_ = true;
        }

        void unlock( )
        {
            locked_ = false;
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
        std::set<sptr> children_;
        bool locked_ = false;
    };
}

#endif // ENVIROMENT_H
