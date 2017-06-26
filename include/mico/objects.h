#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>

namespace mico { namespace objects {

    enum class type {
        BASE = 0,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        TABLE,
        ARRAY,
        NULL_OBJ,
    };

    struct cast {
        template <typename ToT, typename FromT>
        static
        ToT *to( FromT *obj )
        {
            return static_cast<ToT *>(obj);
        }
    };

    struct base {
        virtual ~base( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;

    };

    struct null: public base {

        virtual type get_type( ) const
        {
            return type::NULL_OBJ;
        }

        std::string str( ) const
        {
            return "null";
        }
    };

    using sptr = std::shared_ptr<base>;
    using uptr = std::unique_ptr<base>;

    template <type TName>
    struct type2object;


    template <>
    struct type2object<type::STRING> {
        using native_type = std::string;
    };

    template <>
    struct type2object<type::ARRAY> {
        using native_type = std::vector<sptr>;
    };
    template <>
    struct type2object<type::BOOLEAN> {
        using native_type = bool;
    };

    template <>
    struct type2object<type::FLOAT> {
        using native_type = double;
    };

    template <>
    struct type2object<type::INTEGER> {
        using native_type = std::int64_t;
    };

    template <type TValue>
    class primitive: public base  {
    public:

        using sptr = std::shared_ptr<primitive<TValue> >;

        static const type type_name = TValue;
        using value_type = typename type2object<type_name>::native_type;

        primitive(value_type val)
            :value_(val)
        { }

        type get_type( ) const override
        {
            return type_name;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << std::boolalpha << value( );
            return oss.str( );
        }

        value_type value( ) const
        {
            return value_;
        }

        void set_value( value_type val )
        {
            value_ = val;
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<primitive<type_name> >(std::move(val));
        }

    private:

        value_type value_;
    };

    class string;
    class table;
    class array;

    class string: public base  {
    public:

        using sptr = std::shared_ptr<string>;

        static const type type_name = type::STRING;
        using value_type = type2object<type_name>::native_type;

        string(value_type val)
            :value_(std::move(val))
        { }

        type get_type( ) const override
        {
            return type_name;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "\"" << value( ) << "\"";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        void set_value( value_type val )
        {
            value_ = std::move(val);
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<string>(std::move(val));
        }

    private:

        value_type value_;

    };

    class array: public base {
    public:

        static const type type_name = type::ARRAY;
        using value_type = std::vector<sptr>;

        type get_type( ) const override
        {
            return type_name;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "<array>";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return values_;
        }

        value_type &value( )
        {
            return values_;
        }

        static
        sptr make( )
        {
            return std::make_shared<array>( );
        }

    private:
        value_type values_;
    };

    struct obj_less {

        template <typename ValT>
        static
        bool compare( const sptr &lft, const sptr &rght )
        {
            return cast::to<ValT>(lft.get( ))->value( ) <
                   cast::to<ValT>(rght.get( ))->value( );
        }

        static
        bool comparable( type otl, type otr )
        {
            return
                 ( (otl == type::FLOAT)
                || (otl == type::INTEGER)
                 )
             &&
                 ( (otr == type::FLOAT)
                || (otr == type::INTEGER)
                 )
                    ;
        }

        template <typename LeftT>
        static
        bool compare_numbers( const sptr &lft, const sptr &rght )
        {
            using signed_int   = primitive<type::INTEGER>;
            using floating     = primitive<type::FLOAT>;

            auto lval = cast::to<LeftT>(lft.get( ));
            switch (rght->get_type( )) {
            case type::INTEGER:
                return lval->value( ) < static_cast<typename LeftT::value_type>
                            (cast::to<signed_int>(rght.get( ))->value( ));
            case type::FLOAT:
                return lval->value( ) < static_cast<typename LeftT::value_type>
                            (cast::to<floating>(rght.get( ))->value( ));
                break;
            }
            return false;
        }

        bool operator ( )( const sptr &lft, const sptr &rght ) const
        {
            using boolean      = primitive<type::BOOLEAN>;
            using signed_int   = primitive<type::INTEGER>;
            using floating     = primitive<type::FLOAT>;

            if( lft->get_type( ) == rght->get_type( ) ) {
                switch (lft->get_type( )) {
                case type::BOOLEAN:
                    return compare<boolean>( lft, rght );
                case type::INTEGER:
                    return compare<signed_int>( lft, rght );
                case type::FLOAT:
                    return compare<floating>( lft, rght );
                case type::STRING:
                    return compare<string>( lft, rght );
                case type::ARRAY:
                    return compare<array>( lft, rght );
                case type::TABLE:
                    return compare<table>( lft, rght );
                case type::NULL_OBJ:
                case type::BASE:
                    return false;
                }
            } else if( comparable(lft->get_type( ), rght->get_type( ) ) ){
                switch ( lft->get_type( ) ) {
                case type::INTEGER:
                    return compare_numbers<signed_int>( lft, rght );
                case type::FLOAT:
                    return compare_numbers<floating>( lft, rght );
                default:
                    break;
                }
                return false;
            } else {
                return lft->get_type( ) < rght->get_type( );
            }
        }
    };

    template <>
    struct type2object<type::TABLE> {
        using native_type = std::map<sptr, sptr, obj_less>;
    };

    class table: public base {
    public:

        static const type type_name = type::TABLE;
        using value_type = std::map<sptr, sptr, obj_less>;

        type get_type( ) const override
        {
            return type_name;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "<table>";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return values_;
        }

        value_type &value( )
        {
            return values_;
        }

        static
        sptr make( )
        {
            return std::make_shared<table>( );
        }

    private:
        value_type values_;

    };

    using boolean   = primitive<type::BOOLEAN>;
    using integer   = primitive<type::INTEGER>;
    using floating  = primitive<type::FLOAT>;


    template <typename ObjT>
    struct object2type;

    template <>
    struct object2type<boolean> {
        using object_type = boolean;
        static const type type_name = type::BOOLEAN;
        using native_type = typename type2object<type_name>::native_type;
    };

    template <>
    struct object2type<integer> {
        using object_type = integer;
        static const type type_name = type::INTEGER;
        using native_type = typename type2object<type_name>::native_type;
    };

    template <>
    struct object2type<floating> {
        using object_type = floating;
        static const type type_name = type::FLOAT;
        using native_type = typename type2object<type_name>::native_type;
    };

    template <>
    struct object2type<string> {
        using object_type = string;
        static const type type_name = type::STRING;
        using native_type = typename type2object<type_name>::native_type;
    };

    template <>
    struct object2type<table> {
        using object_type = table;
        static const type type_name = type::TABLE;
        using native_type = typename type2object<type_name>::native_type;
    };

    template <>
    struct object2type<array> {
        using object_type = array;
        static const type type_name = type::ARRAY;
        using native_type = typename type2object<type_name>::native_type;
    };


}}

#endif // OBJECTS_H
