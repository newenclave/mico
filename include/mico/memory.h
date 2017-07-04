#ifndef MEMORY_H
#define MEMORY_H

#include <memory>

namespace mico {

    template <typename T>
    class shared_ptr {

    public:

        using value_type = T;

        shared_ptr( std::shared_ptr<value_type> val,
                    std::shared_ptr<int> cnt )
            :value_(val)
            ,count_(cnt)
        { }

        shared_ptr( ) = default;
        shared_ptr( shared_ptr && ) = default;
        shared_ptr( const shared_ptr & ) = default;
        shared_ptr& operator = ( shared_ptr && ) = default;
        shared_ptr& operator = ( const shared_ptr & ) = default;

        template <typename U>
        shared_ptr( const shared_ptr<U> &o )
            :value_(o.value_)
            ,count_(o.count_)
        { }

        template <typename U>
        shared_ptr& operator = ( const shared_ptr<U> &o )
        {
            shared_ptr<T> tmp(o);
            swap( o );
            return *this;
        }


        value_type *get( )
        {
            return value_.get( );
        }

        const value_type *get( ) const
        {
            return value_.get( );
        }

        std::size_t use_count( ) const
        {
            return value_.use_count( );
        }

        std::size_t total_count( ) const
        {
            return count_.use_count( );
        }

        void reset( )
        {
            value_.reset( );
            count_.reset( );
        }

        template <typename ...Args>
        static
        shared_ptr make( Args&&...args )
        {
            auto val = std::make_shared<T>( std::forward<Args>(args)... );
            auto cnt = std::make_shared<int>( 0 );
            return shared_ptr(val, cnt);
        }

        std::shared_ptr<value_type> value( )
        {
            return value_;
        }

        std::shared_ptr<value_type> count( )
        {
            return count_;
        }

        operator bool ( ) const
        {
            return value_.operator bool( );
        }


        void swap( shared_ptr &o )
        {
            value_.swap( o.value_ );
            count_.swap( o.count_ );
        }

    private:

        std::shared_ptr<value_type>  value_;
        std::shared_ptr<int> count_;
    };

    template <typename T, typename ...Args>
    inline
    shared_ptr<T> make_shared( Args&&...args )
    {
        return shared_ptr<T>::make(std::forward<Args>(args)...);
    }


    template <typename T>
    class weak_ptr {

    public:
        using value_type = T;

        weak_ptr( ) = default;
        weak_ptr( weak_ptr && ) = default;
        weak_ptr( const weak_ptr & ) = default;
        weak_ptr& operator = ( weak_ptr && ) = default;
        weak_ptr& operator = ( const weak_ptr & ) = default;

        template <typename U>
        weak_ptr( shared_ptr<U> shar )
            :value_(shar.value( ))
            ,count_(shar.count( ))
        { }

        template <typename U>
        weak_ptr& operator = ( shared_ptr<U> shar )
        {
            weak_ptr<T> tmp(shar);
            swap(tmp);
            return *this;
        }

        shared_ptr<T> lock( )
        {
            return shared_ptr<T>(value_.lock( ), count_);
        }

        std::size_t use_count( ) const
        {
            return value_.use_count( );
        }

        std::size_t total_count( ) const
        {
            return count_.use_count( );
        }

        void swap( weak_ptr &o )
        {
            value_.swap( o.value_ );
            count_.swap( o.count_ );
        }

    private:
        std::weak_ptr<value_type>  value_;
        std::shared_ptr<int> count_;
    };

}

#endif // MEMORY_H
