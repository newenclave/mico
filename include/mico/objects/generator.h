#ifndef MICO_OBJECTS_GENERATOR_H
#define MICO_OBJECTS_GENERATOR_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/none.h"
#include "mico/objects/array.h"
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
        virtual objects::sptr get( ) = 0;
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

            objects::sptr get( ) override
            {
                if( !end( ) ) {
                    return object_->at( id_ )->value( );
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

            objects::sptr get( ) override
            {
                if( !end( ) ) {
                    auto v = static_cast<std::int64_t>(object_->at(id_));
                    return objects::integer::make(v);
                }
                return nullptr;
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

            objects::sptr get( ) override
            {
                if( !end( ) ) {
                    return object_type::make( id_ );
                }
                return nullptr;
            }

            static
            sptr make( value_type start, value_type stop )
            {
                return make( start, stop, ( ( start > stop ) ?  1 : -1 ) );
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

