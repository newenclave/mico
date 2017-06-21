#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include <memory>
#include <string>
#include <sstream>

namespace mico { namespace objects {

    enum class type {
        BASE = 0,
        BOOLEAN,
        SIGNED_INT,
        UNSIGNED_INT,
        FLOAT,
        STRING,
    };

    struct base {
        virtual ~base( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;

        static
        bool is_numeric( base *b )
        {
            return b->get_type( ) == type::SIGNED_INT
                || b->get_type( ) == type::UNSIGNED_INT
                 ;
        }

    };

    using object_sptr = std::shared_ptr<base>;
    using object_uptr = std::unique_ptr<base>;

    template <type TName>
    struct type2object;


    template <type TValue>
    class primitive: public base  {
    public:

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

    private:

        value_type value_;
    };

    class string;

    template <>
    struct type2object<type::STRING> {
        using object_type = string;
        using native_type = std::string;
    };

    class string: public base  {
    public:

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

    private:

        value_type value_;

    };

    using boolean      = primitive<type::BOOLEAN>;
    using signed_int   = primitive<type::SIGNED_INT>;
    using unsigned_int = primitive<type::UNSIGNED_INT>;
    using floating     = primitive<type::FLOAT>;

    template <>
    struct type2object<type::BOOLEAN> {
        using object_type = boolean;
        using native_type = bool;
    };

    template <>
    struct type2object<type::FLOAT> {
        using object_type = floating;
        using native_type = double;
    };

    template <>
    struct type2object<type::UNSIGNED_INT> {
        using object_type = unsigned_int;
        using native_type = std::uint64_t;
    };

    template <>
    struct type2object<type::SIGNED_INT> {
        using object_type = signed_int;
        using native_type = std::int64_t;
    };

}}

#endif // OBJECTS_H
