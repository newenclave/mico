#ifndef MICO_OBJECT_BASE_H
#define MICO_OBJECT_BASE_H

#include <string>
#include <memory>
#include <vector>
#include <algorithm>

namespace mico {
    class environment;
}

namespace mico { namespace objects {

    enum class type {
        NULL_OBJ = 0,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        TABLE,
        ARRAY,
        REFERENCE,
        RETURN,
        FUNCTION,
        TAIL_CALL,
        BUILTIN,
        FAILURE,
    };

    struct name {
        static
        const char *get( type t )
        {
            switch (t) {
            case type::NULL_OBJ   : return "OBJ_NULL";
            case type::BOOLEAN    : return "OBJ_BOOLEAN";
            case type::INTEGER    : return "OBJ_INTEGER";
            case type::FLOAT      : return "OBJ_FLOAT";
            case type::STRING     : return "OBJ_STRING";
            case type::TABLE      : return "OBJ_TABLE";
            case type::ARRAY      : return "OBJ_ARRAY";
            case type::REFERENCE  : return "OBJ_REFERENCE";
            case type::RETURN     : return "OBJ_RETURN";
            case type::FUNCTION   : return "OBJ_FUNCTION";
            case type::BUILTIN    : return "OBJ_BUILTIN";
            case type::TAIL_CALL  : return "OBJ_TAIL_CALL";
            case type::FAILURE    : return "OBJ_FAILURE";
            }
            return "<invalid>";
        }
    };

    struct base {
        virtual ~base( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;

        virtual
        std::uint64_t hash( ) const
        {
            std::hash<std::string> h;
            return h( str( ) )  ;
        }

        virtual
        bool equal( const base *other ) const
        {
            return str( ) == other->str( );
        }

        static
        std::uint64_t hash64(uint64_t x)
        {
            std::hash<std::uint64_t> h;
            return h(x);
        }

        virtual
        std::size_t size( ) const
        {
            return 0;
        }

        static
        bool is_container( const base *o )
        {
            return (o->get_type( ) == type::ARRAY)
                || (o->get_type( ) == type::TABLE)
                 ;
        }

        virtual
        const environment *hold( ) const
        {
            return nullptr;
        }

        virtual
        bool mark_in( const environment * )
        {
            return false;
        }

        virtual
        bool unmark_in( const environment * )
        {
            return false;
        }

        virtual
        std::size_t marked( ) const
        {
            return 0;
        }

        virtual std::shared_ptr<base> clone( ) const = 0;

    };

    template <type TN>
    struct typed_base: public base {
        type get_type( ) const
        {
            return TN;
        }
    };

    template <type>
    class derived;

    using sptr  = std::shared_ptr<base>;
    using uptr  = std::unique_ptr<base>;
    using slist = std::vector<sptr>;
    using ulist = std::vector<uptr>;

    template <type ToT, typename Obj>
    inline
    derived<ToT> *cast( Obj *val )
    {
#if defined(CHECK_CASTS)
        if( ToT != val->get_type( ) ) {
            throw  std::runtime_error( "Bad object* cast" );
        }
#endif
        return static_cast<derived<ToT> *>(val);
    }

    template <type ToT, typename Obj>
    inline
    derived<ToT> *cast( std::shared_ptr<Obj> val )
    {
#if defined(CHECK_CASTS)
        if( ToT != val->get_type( ) ) {
            throw  std::runtime_error( "Bad shared<object> cast" );
        }
#endif
        return static_cast<derived<ToT> *>(val.get( ));
    }

    inline
    std::ostream &operator << ( std::ostream &o, const objects::sptr &obj )
    {
        return o << obj->str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, const objects::base &obj )
    {
        return o << obj.str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, type t )
    {
        return o << name::get(t);
    }

}}

#endif // OBJECT_BASE_H
