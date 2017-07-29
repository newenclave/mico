#ifndef MICO_OBJECTS_GENERATOR_H
#define MICO_OBJECTS_GENERATOR_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/none.h"
#include "mico/objects/array.h"
#include "mico/objects/table.h"
#include "mico/objects/string.h"
#include "mico/objects/numbers.h"
#include "mico/objects/interval.h"

namespace mico { namespace objects {

    template<>
    class impl<type::GENERATOR>: public typed_base<type::GENERATOR> {

        using this_type = impl<type::GENERATOR>;

    public:

        static const type type_value = type::GENERATOR;
        using sptr = std::shared_ptr<this_type>;

        impl<type::GENERATOR>( )
        { }

        ~impl<type::GENERATOR>( )
        { }

        ast::node::uptr to_ast( tokens::position /*pos*/ ) const override
        {
            return ast::expressions::null::make( );
        }

        virtual void reset( ) = 0;
        virtual void next( ) = 0;
        virtual bool end( ) const = 0;
        virtual bool has_next( ) const = 0;

        virtual objects::sptr get_val( ) = 0;
        virtual objects::sptr get_id( ) = 0;

    };

    using generator = impl<type::GENERATOR>;

    namespace generators {

        template <type ObjT>
        struct obj;

        template <>
        struct obj<type::ARRAY>: public generator {

            using this_type = obj<type::ARRAY>;

            obj<type::ARRAY>( objects::array::sptr &obj, std::int64_t step )
                :object_(obj)
                ,step_(step)
                ,id_(step_ < 0 ? -1 : 0)
            { }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(array)";
                return oss.str( );
            }

            objects::sptr clone( ) const override
            {
                return make( object_ );
            }

            void reset( ) override
            {
                id_ = (step_ < 0 ? -1 : 0);
            }

            void next( ) override
            {
                if( !end( ) ) {
                    id_ += step_;
                }
            }

            bool end( ) const override
            {
                return is_end( id_ );
            }

            bool is_end( std::int64_t id ) const
            {
                return !object_->valid_id( id );
            }

            bool has_next( ) const override
            {
                return !end( ) && !is_end( id_ + step_ );
            }

            objects::sptr get_id( )  override
            {
                return objects::integer::make( id_ );
            }

            objects::sptr get_val( ) override
            {
                if( !end( ) ) {
                    return object_->at( id_ );
                }
                return nullptr;
            }

            static
            sptr make( objects::array::sptr obj )
            {
                return std::make_shared<this_type>( obj, 1 );
            }

            static
            sptr make( objects::array::sptr obj, std::int64_t step )
            {
                return std::make_shared<this_type>( obj, step );
            }

        private:
            array::sptr    object_;
            std::int64_t   step_;
            std::int64_t   id_ = 0;
        };

        using array = obj<type::ARRAY>;


        template <>
        struct obj<type::TABLE>: public generator {

            using this_type = obj<type::TABLE>;
            using iterator  = objects::table::value_type::iterator;
            using const_iterator  = objects::table::value_type::const_iterator;

            obj<type::TABLE>( objects::table::sptr &obj )
                :object_(obj)
            {
                id_ = ibegin( );
            }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(table)";
                return oss.str( );
            }

            objects::sptr clone( ) const override
            {
                return make( object_ );
            }

            void reset( ) override
            {
                id_ = ibegin( );
            }

            void next( ) override
            {
                if( !end( ) ) {
                    ++id_;
                }
            }

            iterator ibegin( )
            {
                return object_->value( ).begin( );
            }

            iterator iend( )
            {
                return object_->value( ).end( );
            }

            const_iterator cbegin( ) const
            {
                return object_->value( ).cbegin( );
            }

            const_iterator cend( ) const
            {
                return object_->value( ).cend( );
            }

            bool end( ) const override
            {
                return is_end( id_ );
            }

            bool is_end( const_iterator id ) const
            {
                return id == cend( );
            }

            bool has_next( ) const override
            {
                return !end( ) && !is_end( std::next( id_ ) );
            }

            objects::sptr get_val( ) override
            {
                if( !end( ) ) {
                    return id_->second;
                }
                return nullptr;
            }

            objects::sptr get_id( ) override
            {
                if( !end( ) ) {
                    return id_->first->clone( );
                }
                return nullptr;
            }

            static
            sptr make( objects::table::sptr obj )
            {
                return std::make_shared<this_type>( obj );
            }

        private:
            table::sptr    object_;
            iterator       id_;
        };

        using table = obj<type::TABLE>;

        template <>
        struct obj<type::STRING>: public generator {

            using this_type = obj<type::STRING>;

            obj<type::STRING>( objects::string::sptr &obj, std::int64_t step )
                :object_(obj)
                ,step_(step)
                ,id_(step_ < 0 ? -1 : 0)
            { }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(string)";
                return oss.str( );
            }

            objects::sptr clone( ) const override
            {
                return make( object_ );
            }

            void reset( ) override
            {
                id_ = (step_ < 0 ? -1 : 0);
            }

            void next( ) override
            {
                if( !end( ) ) {
                    id_ += step_;
                }
            }

            bool end( ) const override
            {
                return is_end( id_ );
            }

            bool is_end( std::int64_t id ) const
            {
                return !object_->valid_id( id );
            }

            bool has_next( ) const override
            {
                return !end( ) && !is_end( id_ + step_ );
            }

            objects::sptr get_val( ) override
            {
                if( !end( ) ) {
                    auto v = static_cast<std::int64_t>(object_->at(id_));
                    return objects::integer::make(v);
                }
                return nullptr;
            }

            objects::sptr get_id( )  override
            {
                return objects::integer::make( id_ );
            }

            static
            sptr make( objects::string::sptr obj, std::int64_t step )
            {
                return std::make_shared<this_type>( obj, step );
            }

            static
            sptr make( objects::string::sptr obj )
            {
                return std::make_shared<this_type>( obj, 1 );
            }

        private:
            objects::string::sptr object_;
            std::int64_t          step_;
            std::int64_t          id_ = 0;
        };
        using string = obj<type::STRING>;

        template <objects::type NumT>
        struct numeric: public generator {

            using object_type = objects::impl<NumT>;
            using this_type   = numeric<NumT>;
            using value_type  = typename object_type::value_type;

            numeric( value_type start, value_type stop, value_type step )
                :start_(start)
                ,stop_(stop)
                ,step_(step)
                ,id_(start)
            { }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(num)";
                return oss.str( );
            }

            objects::sptr clone( ) const override
            {
                return make( start_, stop_, step_ );
            }

            void reset( ) override
            {
                id_ = start_;
            }

            void next( ) override
            {
                if( !end( ) ) {
                    id_ += step_;
                }
            }

            bool end( ) const override
            {
                return is_end( id_ );
            }

            bool is_end( std::int64_t id ) const
            {
                return (step_ < 0) ? ( id <= stop_ ) : ( id >= stop_ );
            }

            bool has_next( ) const override
            {
                return !end( ) && !is_end( id_ + step_ );
            }

            objects::sptr get_val( ) override
            {
                if( !end( ) ) {
                    return object_type::make( id_ );
                }
                return nullptr;
            }

            objects::sptr get_id( ) override
            {
                return object_type::make( id_ );
            }

            static
            sptr make( value_type start, value_type stop )
            {
                return make( start, stop, ( ( start < stop ) ?  1 : -1 ) );
            }

            static
            sptr make( value_type start, value_type stop, value_type step )
            {
                return std::make_shared<this_type>( start, stop, step );
            }

            static
            sptr make( typename objects::intervals::obj<NumT>::sptr &obj )
            {
                return std::make_shared<this_type>( obj->native( ).left( ),
                                                    obj->native( ).right( ) );
            }

            static
            sptr make( typename objects::intervals::obj<NumT>::sptr &obj,
                       value_type step )
            {
                return std::make_shared<this_type>( obj->native( ).left( ),
                                                    obj->native( ).right( ),
                                                    step );
            }

        private:

            value_type start_;
            value_type stop_;
            value_type step_ = 1;
            value_type id_   = 0;
        };

        using integer  = numeric<type::INTEGER>;
        using floating = numeric<type::FLOAT>;
    }
}}

#endif // GENERATOR_H

