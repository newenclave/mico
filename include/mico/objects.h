#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "mico/statements.h"
#include "mico/expressions.h"
//#include "mico/enviroment.h"

namespace mico {

class enviroment;

namespace objects {

    enum class type {
        NULL_OBJ = 0,
        BOOLEAN,
        INTEGER,
        FLOAT,
        STRING,
        TABLE,
        ARRAY,
        RETURN,
        FUNCTION,
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
    using list = std::vector<sptr>;

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

    class retutn_obj: public base {
    public:

        using sptr = std::shared_ptr<retutn_obj>;
        static const type type_name = type::RETURN;
        using value_type = objects::sptr;

        retutn_obj( value_type val )
            :value_(val)
        { }

        type get_type( ) const override
        {
            return type_name;
        }

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

    private:
        value_type value_;
    };

    class function: public base {
    public:

        using sptr = std::shared_ptr<function>;
        static const type type_name = type::FUNCTION;

        function( std::shared_ptr<enviroment> e,
                  std::shared_ptr<ast::expression_list> par,
                  std::shared_ptr<ast::statement_list> st )
            :env_(e)
            ,params_(par)
            ,body_(st)
        { }

        type get_type( ) const override
        {
            return type_name;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(" << params_->size( ) << ")";
            return oss.str( );
        }

        ast::expression_list &params( )
        {
            return *params_;
        }

        const ast::expression_list &params( ) const
        {
            return *params_;
        }

        const ast::statement_list &body( ) const
        {
            return *body_;
        }

        ast::statement_list &body( )
        {
            return *body_;
        }

        std::shared_ptr<enviroment> env( )
        {
            return env_;
        }

        const std::shared_ptr<enviroment> env( ) const
        {
            return env_;
        }

    private:

        std::shared_ptr<enviroment> env_;
        std::shared_ptr<ast::expression_list> params_;
        std::shared_ptr<ast::statement_list>  body_;
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
            using integer   = primitive<type::INTEGER>;
            using floating  = primitive<type::FLOAT>;

            auto lval = cast::to<LeftT>(lft.get( ));
            switch (rght->get_type( )) {
            case type::INTEGER:
                return lval->value( ) < static_cast<typename LeftT::value_type>
                            (cast::to<integer>(rght.get( ))->value( ));
            case type::FLOAT:
                return lval->value( ) < static_cast<typename LeftT::value_type>
                            (cast::to<floating>(rght.get( ))->value( ));
            default:
                break;
            }
            return false;
        }

        bool operator ( )( const sptr &lft, const sptr &rght ) const
        {
            using boolean   = primitive<type::BOOLEAN>;
            using integer   = primitive<type::INTEGER>;
            using floating  = primitive<type::FLOAT>;

            if( lft->get_type( ) == rght->get_type( ) ) {
                switch (lft->get_type( )) {
                case type::BOOLEAN:
                    return compare<boolean>( lft, rght );
                case type::INTEGER:
                    return compare<integer>( lft, rght );
                case type::FLOAT:
                    return compare<floating>( lft, rght );
                case type::STRING:
                    return compare<string>( lft, rght );
                case type::ARRAY:
                    return compare<array>( lft, rght );
                case type::TABLE:
                    return compare<table>( lft, rght );
                case type::NULL_OBJ:
                    return false;
                }
            } else if( comparable(lft->get_type( ), rght->get_type( ) ) ){
                switch ( lft->get_type( ) ) {
                case type::INTEGER:
                    return compare_numbers<integer>( lft, rght );
                case type::FLOAT:
                    return compare_numbers<floating>( lft, rght );
                default:
                    break;
                }
                return false;
            } else {
                return lft->get_type( ) < rght->get_type( );
            }
            return false;
        }
    };

    template <>
    struct type2object<type::TABLE> {
        using native_type = std::map<objects::sptr, objects::sptr, obj_less>;
    };

    class table: public base {
    public:

        using sptr = std::shared_ptr<table>;
        static const type type_name = type::TABLE;
        using value_type = std::map<objects::sptr, objects::sptr, obj_less>;

        type get_type( ) const override
        {
            return type_name;
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "{ ";
            for( auto &v: values_ ) {
                oss << v.first->str( ) << ":"
                    << v.second->str( )<< " ";
            }
            oss << "}";
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
