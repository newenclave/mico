#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include <memory>
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

    class boolean: public base {
    public:

        using value_type = bool;

        boolean( bool val )
            :value_(val)
        { }

        bool value( ) const
        {
            return value_;
        }

        std::string str( ) const override
        {
            return value( ) ? "true" : "false";
        }

        type get_type( ) const override
        {
            return type::BOOLEAN;
        }

    private:
        bool value_;
    };

    template <typename IntT, type TValue>
    class numeric: public base  {
    public:

        using value_type = IntT;

        numeric(value_type val)
            :value_(val)
        { }

        type get_type( ) const override
        {
            return TValue;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << value( );
            return oss.str( );
        }

        value_type value( ) const
        {
            return value_;
        }

    private:

        value_type value_;
    };

    class signed_int: public numeric<std::int64_t, type::SIGNED_INT> {
    public:
        using super_type = numeric<std::int64_t, type::SIGNED_INT>;
        using value_type = std::int64_t;
        signed_int(value_type val )
            :super_type(val)
        { }
    };

    class unsigned_int: public numeric<std::uint64_t, type::UNSIGNED_INT> {
    public:
        using super_type = numeric<std::uint64_t, type::UNSIGNED_INT>;
        using value_type = std::uint64_t;
        unsigned_int(value_type val )
            :super_type(val)
        { }
    };

    class floating: public numeric<double, type::FLOAT> {
    public:
        using super_type = numeric<double, type::FLOAT>;
        using value_type = double;
        floating(double val )
            :super_type(val)
        { }
    };

}}

#endif // OBJECTS_H
