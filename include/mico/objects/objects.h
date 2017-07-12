#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

#include "mico/statements.h"
#include "mico/expressions.h"
#include "mico/environment.h"

#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/numbers.h"
#include "mico/objects/null.h"
#include "mico/objects/table.h"
#include "mico/objects/array.h"
#include "mico/objects/functions.h"

namespace mico { namespace objects {

    template <>
    class derived<type::STRING>: public typed_base<type::STRING> {

        using this_type = derived<type::STRING>;

    public:

        using sptr = std::shared_ptr<this_type>;
        using value_type = std::string;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "\"" << value( ) << "\"";
            return oss.str( );
        }
        derived( value_type val )
            :value_(std::move(val))
        { }
        const value_type &value( ) const
        {
            return value_;
        }
        value_type &value( )
        {
            return value_;
        }

        std::size_t hash( ) const override
        {
            std::hash<std::string> h;
            return h(value_);
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return o->value( ) == value( );
            }
            return false;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( value_ );
        }

    private:
        value_type value_;
    public:
    };

    template <>
    class derived<type::RETURN>: public typed_base<type::RETURN> {
        using this_type = derived<type::RETURN>;
    public:
        using sptr = std::shared_ptr<this_type>;

        using value_type = objects::sptr;

        derived( value_type val )
            :value_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "return " << value( )->str( );
            return oss.str( );
        }

        value_type &value( )
        {
            return value_;
        }

        const value_type &value( ) const
        {
            return value_;
        }

        static
        sptr make( objects::sptr res )
        {
            return std::make_shared<this_type>( res );
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( value_ );
        }

    private:
        value_type value_;
    };

    template <>
    class derived<type::BOOLEAN>: public typed_base<type::BOOLEAN> {
        using this_type = derived<type::BOOLEAN>;

        struct key { };

    public:

        using sptr = std::shared_ptr<this_type>;

        derived( bool v )
            :value_(v)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << (value_ ? "true" : "false");
            return oss.str( );
        }

        bool value( ) const
        {
            return value_;
        }

        static
        sptr make( bool val )
        {
            static auto true_this  = std::make_shared<this_type>( true );
            static auto false_this = std::make_shared<this_type>( false );
            return val ? true_this : false_this;
        }

        std::uint64_t hash( ) const override
        {
            return value_ ? 1 : 0;
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return o->value( ) == value( );
            }
            return false;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return make( value_ );
        }

    private:

        bool value_;
    };

    template <>
    class derived<type::ERROR>: public typed_base<type::ERROR> {

        using this_type = derived<type::ERROR>;
    public:
        using sptr = std::shared_ptr<this_type>;

        using value_type = std::string;

        derived( tokens::position where, value_type val )
            :where_(where)
            ,value_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "error: [" << where_ << "] " << value( );
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            auto res = make( where_, value_ );
            return res;
        }

    private:

        static
        void out_err( std::ostream & ) { }

        template <typename HeadT, typename ...Rest>
        static
        void out_err( std::ostream &o, const HeadT &h, Rest&&...rest )
        {
            o << h;
            out_err( o, std::forward<Rest>(rest)... );
        }
    public:

        template <typename ...Args>
        static
        objects::sptr make( const ast::node *n, Args&&...args )
        {
            std::ostringstream oss;
            out_err( oss, std::forward<Args>(args)...);
            return make( n->pos( ), oss.str( ) );
        }

        template <typename ...Args>
        static
        objects::sptr make( tokens::position where, Args&&...args )
        {
            std::ostringstream oss;
            out_err( oss, std::forward<Args>(args)...);
            return std::make_shared<this_type>( where, oss.str( ) );
        }

        template <typename ...Args>
        static
        objects::sptr make( Args&&...args )
        {
            std::ostringstream oss;
            out_err( oss, std::forward<Args>(args)... );
            tokens::position pos(0, 0);
            return make(pos, oss.str( ) );
        }

    private:
        tokens::position where_;
        value_type value_;
    };

    using null       = derived<type::NULL_OBJ>;
    using string     = derived<type::STRING>;
    using function   = derived<type::FUNCTION>;
    using builtin    = derived<type::BUILTIN>;
    using cont_call  = derived<type::CONT_CALL>;
    using retutn_obj = derived<type::RETURN>;
    using boolean    = derived<type::BOOLEAN>;
    using integer    = derived<type::INTEGER>;
    using floating   = derived<type::FLOAT>;
    using array      = derived<type::ARRAY>;
    using reference  = derived<type::REFERENCE>;
    using table      = derived<type::TABLE>;
    using error      = derived<type::ERROR>;

    inline
    std::ostream &operator << ( std::ostream &o, const objects::sptr &obj )
    {
        return o << obj->str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, type t )
    {
        return o << name::get(t);
    }

}}

#endif // OBJECTS_H
