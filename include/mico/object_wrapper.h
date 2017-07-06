#ifndef MICO_OBJECT_WRAPPER_H
#define MICO_OBJECT_WRAPPER_H

#include <memory>
#include "mico/object_base.h"

namespace mico { namespace objects {

    template <typename T = base>
    class wrapper {

    public:

        using value_type = std::shared_ptr<T>;

        wrapper( ) = default;

        ~wrapper( )
        {
            if( value_ ) {
                value_->on_destroy( );
            }
        }

        wrapper( const wrapper &other )
            :value_(other.value_)
        {
            value_->on_copy( );
            lf_ = value_->lock_factor( );
        }

        wrapper( std::nullptr_t )
        { }

        wrapper( wrapper &&other )
            :value_(other.value_)
        {
            other.value_.reset( );
        }

        wrapper& operator = ( const wrapper &other )
        {
            value_ = other.value_;
            value_->on_copy( );
            lf_ = value_->lock_factor( );
            return *this;
        }

        wrapper& operator = ( wrapper &&other )
        {
            value_ = other.value_;
            other.value_.reset( );
            lf_ = value_->lock_factor( );
            return *this;
        }

        wrapper& operator = ( const value_type &val )
        {
            wrapper tmp(val);
            swap( tmp );
        }

        wrapper( const value_type &val )
            :value_(val)
        {
            value_->on_copy( );
            lf_ = value_->lock_factor( );
        }

        operator bool ( ) const
        {
            return value_.operator bool( );
        }

        void swap( wrapper &o )
        {
            value_.swap( o.value_ );
            std::swap( lf_, o.lf_ );
        }

        objects::base *operator -> ( )
        {
            return  value_.get( );
        }

        const objects::base *operator -> ( ) const
        {
            return  value_.get( );
        }

        objects::base *get( )
        {
            return  value_.get( );
        }

        const objects::base *get( ) const
        {
            return  value_.get( );
        }

        const objects::base &operator * ( ) const
        {
            return  *value_;
        }

        objects::base &operator * ( )
        {
            return  *value_;
        }

        value_type &value( )
        {
            return value_;
        }

        const value_type &value( ) const
        {
            return value_;
        }

        void reset( )
        {
            if(value_) {
                value_->on_destroy( );
                value_.reset( );
            }
        }

        std::size_t lock_factor( ) const
        {
            return lf_;
        }

        template <typename DerivedT, typename ...Args>
        static
        wrapper make( Args...args )
        {
            auto res = std::make_shared<DerivedT>(
                                    std::forward<Args>(args)... );
            return wrapper(res);
        }

    private:

        std::size_t lf_;

        value_type value_;
    };

} }


#endif // MICO_OBJECT_WRAPPER_H
