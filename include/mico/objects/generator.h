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
#include "mico/objects/character.h"

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

            explicit
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
                    return objects::character::make(object_->at(id_));
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
            using ival_type   = etool::intervals::interval<value_type>;

            static
            ival_type make_ival( value_type start, value_type stop )
            {
                return (start < stop)
                        ? ival_type::left_closed(start, stop)
                        : ival_type::right_closed(stop, start);
            }

            numeric( value_type stop, value_type step )
                :ival_(make_ival( 0, stop ))
                ,step_(step)
                ,id_(0)
            { }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(num)";
                return oss.str( );
            }

            objects::sptr clone( ) const override
            {
                return make( ival_.right( ), step_ );
            }

            void reset( ) override
            {
                id_ = ival_.left( );
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

            bool is_end( value_type id ) const
            {
                return !ival_.contains(id);
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
            sptr make( value_type stop )
            {
                return make( stop, ( ( 0 < stop ) ?  1 : -1 ) );
            }

            static
            sptr make( value_type stop, value_type step )
            {
                return std::make_shared<this_type>( stop, step );
            }

        private:
            ival_type  ival_;
            value_type step_ = 1;
            value_type id_   = 0;
        };

        template <objects::type NumT>
        struct interval: public generator {

            using object_type = objects::impl<NumT>;
            using this_type   = interval<NumT>;
            using value_type  = typename object_type::value_type;
            using ival_type   = etool::intervals::interval<value_type>;

            static
            ival_type make_ival( value_type start, value_type stop )
            {
                return (start < stop)
                        ? ival_type::closed(start, stop)
                        : ival_type::closed(stop, start);
            }

            interval( value_type start, value_type stop, value_type step )
                :ival_(make_ival(start, stop))
                ,step_(step)
                ,id_(start)
            { }

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "gen(ival)";
                return oss.str( );
            }

            objects::sptr clone( ) const override
            {
                return make( ival_.left( ), ival_.right( ), step_ );
            }

            void reset( ) override
            {
                id_ = ival_.left( );
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

            bool is_end( value_type id ) const
            {
                return !ival_.contains( id );
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
            sptr make( value_type start, value_type stop, value_type step )
            {
                return std::make_shared<this_type>( start, stop, step );
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
            ival_type  ival_;
            value_type step_ = 1;
            value_type id_   = 0;
        };


        using integer  = numeric<type::INTEGER>;
        using floating = numeric<type::FLOAT>;

        using int_ival   = interval<type::INTEGER>;
        using float_ival = interval<type::FLOAT>;
    }
}}

#endif // GENERATOR_H

