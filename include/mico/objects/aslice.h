#ifndef MICO_OBJECTS_SLICE_ARR_H
#define MICO_OBJECTS_SLICE_ARR_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/objects/character.h"
#include "mico/objects/array.h"

#include "mico/types.h"

namespace mico { namespace objects {

    template <>
    class impl<type::ASLICE>: public typed_base<type::ASLICE> {
        using this_type = impl<type::ASLICE>;
    public:

        static const type type_value = type::ASLICE;
        using sptr = std::shared_ptr<this_type>;
        using value_type = objects::array;

        impl<type::ASLICE>( value_type::sptr str,
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
            oss << "array_slice[" << start_ << ".." << stop_ - 1 << "]";
            return oss.str( );
        }

        static
        sptr make( value_type::sptr str,
                   std::size_t start, std::size_t stop )
        {
            auto val = std::make_shared<this_type>( str, start, stop );
            return val;
        }

        hash_type hash( ) const override
        {
            return 0;
        }

        std::size_t size( ) const override
        {
            return stop_ - start_;
        }

        value_type::sptr &value( )
        {
            return obj_;
        }

        const value_type::sptr &value( ) const
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

       objects::sptr at( std::int64_t id ) const
        {
            auto fixed = fix_id( id );
            return obj_->value(  )[fixed];
        }

        bool equal( const base *o ) const override
        {
            return (o->get_type( ) == get_type( ));
        }

        objects::sptr clone( ) const override
        {
            return make( obj_, start_, stop_ );
        }

        ast::node::uptr to_ast( tokens::position /*pos*/ ) const override
        {
            return nullptr;
        }

    private:
        value_type::sptr obj_;
        std::size_t      start_;
        std::size_t      stop_;
        bool             negative_ = false;
    };

    using aslice = impl<type::ASLICE>;

}}

#endif // SLICE_STR_H
