#ifndef MICO_OBJECTS_SLICE_STR_H
#define MICO_OBJECTS_SLICE_STR_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/objects/character.h"
#include "mico/objects/string.h"
#include "mico/objects/array.h"

#include "mico/types.h"

namespace mico { namespace objects {

    template <typename T, type TName>
    class slice_impl: public typed_base<TName> {
        using this_type = slice_impl<T, TName>;
    public:

        static const type type_value = TName;
        using sptr                   = std::shared_ptr<this_type>;
        using value_type             = typename T::sptr;

        slice_impl<T, TName>( value_type str,
                              std::size_t start, std::size_t stop )
            :obj_(str)
            ,start_(start < stop ? start : stop)
            ,stop_(start < stop ? stop : start)
            ,negative_(stop < start)
        {
            stop_++;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "slice[" << start_ << ".." << stop_ << "]";
            return oss.str( );
        }

        base::hash_type hash( ) const override
        {
            return 0;
        }

        std::size_t size( ) const override
        {
            return stop_ - start_;
        }

        value_type &value( )
        {
            return obj_;
        }

        const value_type &value( ) const
        {
            return obj_;
        }

        std::size_t fix_id( std::int64_t id ) const
        {
            if( negative_ ) {
                id = (id * -1) - 1;
            }

            auto siz = static_cast<std::size_t>(id);
            if( id < 0 ) {
                return ( obj_->size( ) - static_cast<std::size_t>(-1 * id))
                     - ( obj_->size( ) - stop_ );
            }
            return siz + start_;
        }

        bool valid_id( std::int64_t id ) const
        {
            auto fixed = fix_id(id);
            return ( fixed < obj_->size( ) )
                && ( fixed < stop_ )
                && ( fixed >= start_ )
                 ;
        }

        virtual
        objects::sptr at( std::int64_t id ) const = 0;

        bool equal( const base *o ) const override
        {
            return (o->get_type( ) == this->get_type( ));
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using infix = ast::expressions::infix;
            using intgr = ast::expressions::integer;
            using index = ast::expressions::index;

            auto start = static_cast<std::int64_t>(negative_ ? stop_ : start_);
            auto stop  = static_cast<std::int64_t>(negative_ ? start_ : stop_);

            auto lft = obj_->to_ast( pos );

            auto param = infix::make( tokens::type::DOTDOT, intgr::make(start));
            param->set_right( intgr::make( stop ) );

            auto idx = index::make(std::move(lft), std::move(param));
            return ast::node::uptr(std::move(idx));
        }

        std::size_t begin( )  const
        {
            return start_;
        }

        std::size_t end( ) const
        {
            return stop_;
        }

    private:
        value_type  obj_;
        std::size_t start_;
        std::size_t stop_;
        bool        negative_ = false;
    };

    template <>
    class impl<type::SSLICE>: public slice_impl<objects::string, type::SSLICE> {
        using this_type = impl<type::SSLICE>;
    public:

        using sptr = std::shared_ptr<this_type>;

        impl<type::SSLICE>( objects::string::sptr obj,
                            std::size_t start, std::size_t stop )
            :slice_impl<objects::string, type::SSLICE>(obj, start, stop)
        { }

        objects::sptr at( std::int64_t id ) const override
        {
            auto fixed = fix_id( id );
            return objects::character::make(value( )->value( )[fixed]);
        }

        static
        sptr make( objects::string::sptr obj,
                   std::size_t start, std::size_t stop )
        {
            auto val = std::make_shared<this_type>( obj, start, stop );
            return val;
        }

        objects::sptr clone( ) const override
        {
            return make( value( ), begin( ), end( ) );
        }

        mico::string make_string( ) const
        {
            auto &s(value( )->value( ));
            auto b = static_cast<mico::string::difference_type>(begin( ));
            auto e = static_cast<mico::string::difference_type>(end( ));
            return mico::string( s.begin( ) + b, s.begin( ) + e );
        }

    };

    template <>
    class impl<type::ASLICE>: public slice_impl<objects::array, type::ASLICE> {
        using this_type = impl<type::ASLICE>;
    public:

        using sptr = std::shared_ptr<this_type>;

        impl<type::ASLICE>( objects::array::sptr obj,
                            std::size_t start, std::size_t stop )
            :slice_impl<objects::array, type::ASLICE>(obj, start, stop)
        { }

        objects::sptr at( std::int64_t id ) const override
        {
            auto fixed = fix_id( id );
            return value( )->value( )[fixed];
        }

        static
        sptr make( objects::array::sptr obj,
                   std::size_t start, std::size_t stop )
        {
            auto val = std::make_shared<this_type>( obj, start, stop );
            return val;
        }

        objects::sptr clone( ) const override
        {
            return make( value( ), begin( ), end( ) );
        }
    };

    using sslice = impl<type::SSLICE>;
    using aslice = impl<type::ASLICE>;

}}

#endif // SLICE_STR_H
