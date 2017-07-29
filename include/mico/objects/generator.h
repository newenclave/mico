#ifndef MICO_OBJECTS_GENERATOR_H
#define MICO_OBJECTS_GENERATOR_H


#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/none.h"
#include "mico/objects/array.h"
#include "mico/objects/string.h"
#include "mico/objects/numbers.h"

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
        virtual objects::sptr get( ) = 0;
    };

    using generator = impl<type::GENERATOR>;

    namespace gens {
        template <type ObjT>
        struct obj_impl;

        template <>
        struct obj_impl<type::ARRAY>: public generator {

            using this_type = obj_impl<type::ARRAY>;

            obj_impl<type::ARRAY>( objects::array::sptr &obj )
                :object_(obj)
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
                id_ = 0;
            }

            void next( ) override
            {
                if( id_ < object_->size( ) ) {
                    ++id_;
                }
            }

            bool end( ) const override
            {
                return id_ == object_->size( );
            }

            objects::sptr get( ) override
            {
                if( id_ < object_->size( ) ) {
                    return object_->value( )[id_]->value( );
                }
                return nullptr;
            }

            static
            sptr make( objects::array::sptr obj )
            {
                return std::make_shared<this_type>( obj );
            }

        private:
            array::sptr object_;
            std::size_t id_ = 0;
        };
        using array = obj_impl<type::ARRAY>;

        template <>
        struct obj_impl<type::STRING>: public generator {

            using this_type = obj_impl<type::STRING>;

            obj_impl<type::STRING>( objects::string::sptr &obj )
                :object_(obj)
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
                id_ = 0;
            }

            void next( ) override
            {
                if( id_ < object_->size( ) ) {
                    ++id_;
                }
            }

            bool end( ) const override
            {
                return id_ == object_->size( );
            }

            objects::sptr get( ) override
            {
                if( id_ < object_->size( ) ) {
                    auto v = static_cast<std::int64_t>(object_->value( )[id_]);
                    return objects::integer::make(v);
                }
                return nullptr;
            }

            static
            sptr make( objects::string::sptr obj )
            {
                return std::make_shared<this_type>( obj );
            }

        private:
            objects::string::sptr object_;
            std::size_t           id_ = 0;
        };
        using string = obj_impl<type::ARRAY>;

        template <typename NumT>
        struct numeric: public generator {

            using this_type  = numeric;
            using value_type = typename NumT::value_type;

            numeric( value_type start, value_type stop,
                     value_type step = 1 )
                :start_(start)
                ,stop_(stop)
                ,step_(step)
            { }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(int)";
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
                if( id_ != stop_ ) {
                    id_ += step_;
                }
            }

            bool end( ) const override
            {
                return (step_ < 0) ? (id_ <= stop_) : (id_ >= stop_);
            }

            objects::sptr get( ) override
            {
                if( !end( ) ) {
                    return NumT::make( id_ );
                }
                return nullptr;
            }

            static
            sptr make( value_type start, value_type stop )
            {
                return make( start, stop, 1 );
            }

            static
            sptr make( value_type start, value_type stop,
                       value_type step )
            {
                return std::make_shared<this_type>( start, stop, step );
            }

        private:

            value_type start_;
            value_type stop_;
            value_type step_ = 1;
            value_type id_   = 0;
        };

        using integer  = numeric<objects::integer>;
        using floating = numeric<objects::floating>;
    }
}}

#endif // GENERATOR_H

