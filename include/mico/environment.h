#ifndef MICO_ENVIROMENT_H
#define MICO_ENVIROMENT_H

#include <set>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <memory>

#include "mico/objects/base.h"
#include "mico/objects/reference.h"

#include "etool/console/colors.h"

namespace mico {

#define DEBUG 0
#define CHECK_ENV_PARENTS 0

#if DEBUG
    static int c = 0;
#endif

    struct state;

    class environment: public std::enable_shared_from_this<environment> {

    public:

        using sptr          = std::shared_ptr<environment>;
        using wptr          = std::weak_ptr<environment>;
        using object_sptr   = std::shared_ptr<objects::base>;
        using object_wptr   = std::weak_ptr<objects::base>;
        using children_type = std::set<sptr>;
        using obj_reference = objects::impl<objects::type::REFERENCE>;
        using data_map      = std::map<std::string, obj_reference::sptr>;
        using data_set      = std::set<obj_reference::sptr>;
        using parent_list   = std::list<wptr>;

    protected:

        struct key { };

    public:

        struct scoped {

            explicit
            scoped( sptr v )
                :env_(v)
            {
                if( env_ ) {
                    env_->mark( );
                }
            }

            ~scoped(  )
            {
                if( env_ ) {
                    env_->unmark( );
                }
            }

            sptr env( )
            {
                return env_;
            }

        private:
            sptr env_;
        };

        environment( state *st, key )
            :state_(st)
        {
#if DEBUG
            std::cout << ++c << "\n";
#endif
        }

        environment( sptr env, key )
            :state_(env->state_)
            ,parent_(env)
        {
#if DEBUG
            std::cout << ++c << "\n";
#endif
        }

        ~environment( )
        {
#if DEBUG
            std::cout << --c << "\n";
#endif
            data_.clear( );
            children_.clear( );
        }

        void clear( )
        {
            if( children_.empty( ) ) {
                data_.clear( );
            }
        }

        static
        sptr make( state *st )
        {
            return std::make_shared<environment>( st, key( ) );
        }

        static
        sptr make( sptr parent )
        {
            auto res = std::make_shared<environment>( parent, key( ) );
            parent->children_.insert(res);
            return res;
        }

        static
        bool mark_in( environment::sptr remote, const environment *current )
        {
            auto my_env = remote;
            auto e      = current;
            if( my_env ) {
                if( my_env->is_parent( e ) ) {
                    while( my_env.get( ) != e ) {
                        my_env->mark( );
                        my_env = my_env->parent( );
                    }
                } else if( !e->is_parent( my_env.get( ) ) ) {
                    auto par = environment::barrier( e, my_env.get( ) );
#if CHECK_ENV_PARENTS
                    if( !e->is_parent( par ) ) {
                        throw std::logic_error( "Lock. Not a parent!" );
                    }
                    if( !my_env->is_parent( par ) ) {
                        throw std::logic_error( "Lock. Not a parent!" );
                    }
#endif
                    mark_in( remote, par );
                }
                return true;
            }
            return false;
        }

        static
        bool unmark_in( environment::sptr remote, const environment *current )
        {
            auto my_env = remote;
            auto e      = current;

            if( my_env ) {
                if( my_env->is_parent( current ) ) {
                    while( my_env.get( ) != current ) {
                        my_env->unmark( );
                        my_env = my_env->parent( );
                    }
                } else if( !e->is_parent( my_env.get( ) ) ) {
                    auto par = environment::barrier( e, my_env.get( ) );
                    if( par ) {
#if CHECK_ENV_PARENTS
                        if( !e->is_parent( par ) ) {
                            throw std::logic_error( "Unlock. Not a parent!" );
                        }
                        if( !my_env->is_parent( par ) ) {
                            throw std::logic_error( "Unlock. Not a parent!" );
                        }
#endif
                        unmark_in( remote, par );
                    }
                }
                return true;
            }
            return false;
        }

        void drop( sptr child )
        {
            children_.erase( child );
            drop( );
        }

        void drop( )
        {
            if( !marked_ ) {
                auto p = parent( );
                if( p ) {
                    data_.clear( );
                    p->drop( shared_from_this( ) );
                }
            }
        }

        state &get_state( )
        {
            return *state_;
        }

        const state &get_state( ) const
        {
            return *state_;
        }

        parent_list &parents( )
        {
            return parents_;
        }

        const parent_list &parents( ) const
        {
            return parents_;
        }

        sptr parent( )
        {
            return parent_.lock( );
        }

        const sptr parent( ) const
        {
            return parent_.lock( );
        }

        const objects::base *owner( ) const
        {
            return owner_;
        }

        void set_owner( const objects::base *own )
        {
            owner_ = own;
        }

        // lowest common ancestor
        static
        const environment *barrier( const environment *l, const environment *r )
        {
            while( l && !r->is_parent( l ) ) {
                l = l->parent( ).get( );
            }
            return l;
        }

        std::size_t is_parent( const environment *pp ) const
        {
            auto c = this;
            std::size_t count = 0;
            while( c ) {
                ++count;
                if( c == pp ) {
                    return count;
                }
                c = c->parent_.lock( ).get( );
            }
            return 0;
        }

        void mark( )
        {
            marked_++;
        }

        std::size_t marked( ) const
        {
            return marked_;
        }

        void unmark( )
        {
#if CHECK_ENV_PARENTS
            if( marked_ == 0  ) {
                throw std::logic_error( "Unmark. This item is not marked." );
            }
#endif
            marked_--;
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
                    parent = cur->parent( );
                    cur = parent.get( );
                }
            }
            return nullptr;
        }

        void add_parent( sptr par )
        {
            parents_.push_back( par );
        }

        void set( const std::string &name, object_sptr val )
        {
            auto ref = obj_reference::make_var( this, val );
            data_[name] = ref;
        }

        void set_const( const std::string &name, object_sptr val )
        {
            auto ref = obj_reference::make_const( this, val );
            data_[name] = ref;
        }

        void keep( object_sptr val )
        {
            hide_.insert( obj_reference::make_var( this, val ) );
        }

        object_sptr get_here( const std::string &name )
        {
            auto f = data_.find( name );
            if( f != data_.end( ) ) {
                return f->second->is_mutable( )
                     ? f->second
                     : f->second->value( );
            }
            return nullptr;
        }

        object_sptr get_parent( const std::string &name, bool here_only )
        {
            auto b = parents_.begin( );
            auto e = parents_.end( );

            while( b != e ) {
                auto pl = b->lock( );
                if( pl ) {
                    object_sptr v = here_only ? pl->get_parents_only( name )
                                              : pl->get( name );
                    if( v ) {
                        return v;
                    }
                    ++b;
                } else {
                    b = parents_.erase( b );
                }
            }
            return nullptr;
        }

        object_sptr get_parents_only( const std::string &name )
        {
            if( auto v = get_here( name ) ) {
                return v;
            }
            return get_parent( name, true );
        }

        object_sptr get( const std::string &name )
        {
            auto cur = this;
            object_sptr res;
            sptr parent;
            while( cur && !res ) {
                if( auto val = cur->get_here( name ) ) {
                    res = val;
                } else if( auto pr = cur->get_parent( name, false ) ) {
                    res = pr;
                } else {
                    parent = cur->parent( );
                    cur = parent.get( );
                }
            }
            return res;
        }

        data_map &data( )
        {
            return data_;
        }

        children_type &children( )
        {
            return children_;
        }

        void introspect( )
        {
            std::cout << "Root: ";
            introspect( 1 );
        }

        void introspect( std::size_t level )
        {
            using namespace etool::console::ccout;

            std::string space( level * 2, ' ' );
            std::cout << "[" << (marked( ) ? cyan : light)
                      << this << none << "]\n" ;
            for( auto &d: data_ ) {
                char mut = d.second->is_mutable( ) ?'M' : 'C';
                std::cout << space << mut << " " << d.first
                          << " => " << d.second->value( );
                if( d.second->value( )->hold( ) ) {
                    std::cout << " [" << blue
                              << d.second->value( )->hold( )
                              << none << "]"
                                 ;
                }
                std::cout << std::endl;
            }

            std::size_t cnt = 0;
            for( auto &a: hide_ ) {
                std::cout << space << "hidden_" << cnt++
                          << " => " << a->value( );
                if( a->value( )->hold( ) ) {
                    std::cout << " [" << blue
                              << a->value( )->hold( )
                              << none << "]"
                                 ;
                }
                std::cout << std::endl;
            }

            for( auto &c: children_ ) {
                auto cl = c;
                if( !cl ) continue;
                std::cout << space << "Child: " << cl->marked( ) << " ";
                cl->introspect( level + 1 );
            }
        }

        void run_GC( bool deep )
        {
            auto b = children( ).begin( );
            auto e = children( ).end( );
            while( b != e ) {
                auto lck = (*b)->marked( );
                if( 0 == lck ) {
                    b = children( ).erase( b );
                } else {
                    if( deep ) {
                        (*b)->run_GC( deep );
                    }
                    ++b;
                }
            }
            return;
        }

    private:

        state                *state_;
        wptr                  parent_;
        children_type         children_;
        data_map              data_;
        data_set              hide_;
        parent_list           parents_;
        std::size_t           marked_ = 0;
        const objects::base  *owner_ = nullptr;
    };
}

#ifdef CHECK_ENV_PARENTS
#   undef CHECK_ENV_PARENTS
#endif

#endif // ENVIROMENT_H
