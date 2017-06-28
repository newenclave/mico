#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "mico/statements.h"
#include "mico/expressions.h"
#include "mico/enviroment.h"

namespace mico {

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
        CONT_CALL,
        ERROR,
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

    template <type TN>
    struct typed_base: public base {
        type get_type( ) const
        {
            return TN;
        }
    };

    using sptr = std::shared_ptr<base>;
    using uptr = std::unique_ptr<base>;
    using list = std::vector<sptr>;

    template <type TName>
    struct type2object;

    template <>
    struct type2object<type::FLOAT> {
        using native_type = double;
    };

    template <>
    struct type2object<type::INTEGER> {
        using native_type = std::int64_t;
    };

    template <type>
    class derived;

    template <>
    class derived<type::NULL_OBJ>: public typed_base<type::NULL_OBJ> {
        using this_type = derived<type::NULL_OBJ>;
    public:
        using sptr = std::shared_ptr<this_type>;
        std::string str( ) const
        {
            return "null";
        }
    };

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
    private:
        value_type value_;
    public:
    };

    template <type TN>
    class env_object: public typed_base<TN> {

    public:

        env_object( std::shared_ptr<enviroment> e )
            :env_(e)
        { }

        ~env_object( )
        {
            drop( );
        }

        std::shared_ptr<enviroment> env( )
        {
            return env_.lock( );
        }

        const std::shared_ptr<enviroment> env( ) const
        {
            return env_.lock( );
        }

    private:

        void drop( )
        {
            auto p = env_.lock( );
            if( p ) {
                p->drop( );
            }
        }

        std::weak_ptr<enviroment> env_;
    };

    template <>
    class derived<type::FUNCTION>: public env_object<type::FUNCTION> {
        using this_type = derived<type::FUNCTION>;
    public:
        using sptr = std::shared_ptr<this_type>;

        derived( std::shared_ptr<enviroment> e,
                  std::shared_ptr<ast::expression_list> par,
                  std::shared_ptr<ast::statement_list> st )
            :env_object(e)
            ,params_(par)
            ,body_(st)
        { }

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

    private:
        std::shared_ptr<ast::expression_list> params_;
        std::shared_ptr<ast::statement_list>  body_;
    };

    template <>
    class derived<type::CONT_CALL>: public env_object<type::CONT_CALL> {

        using this_type = derived<type::CONT_CALL>;
    public:
        using sptr = std::shared_ptr<this_type>;

        derived(objects::sptr obj, enviroment::sptr e)
            :env_object(e)
            ,obj_(obj)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "cc(" << ")";
            return oss.str( );
        }

        objects::sptr  value( )
        {
            return obj_;
        }

    private:
        objects::sptr obj_;
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

    private:
        value_type value_;
    };

    template <>
    class derived<type::ARRAY>: public typed_base<type::ARRAY> {
        using this_type = derived<type::ARRAY>;
    public:

        using sptr = std::shared_ptr<this_type>;
        using value_type = std::vector<sptr>;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "<array>";
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
            return std::make_shared<this_type>( val );
        }

    private:

        bool value_;
    };

    template <type TN>
    class derived: public typed_base<TN>  {
        using this_type = derived<TN>;
    public:
        using sptr = std::shared_ptr<this_type>;

        static const type type_name = TN;
        using value_type = typename type2object<type_name>::native_type;

        derived(value_type val)
            :value_(val)
        { }

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

        void set_value( value_type val )
        {
            value_ = val;
        }

    private:

        value_type value_;
    };

    struct less
    {
        template <typename ValT>
        static
        bool compare( const sptr &lft, const sptr &rght )
        {
            return cast::to<ValT>(lft.get( ))->value( ) <
                   cast::to<ValT>(rght.get( ))->value( );
        }

        static
        bool compare_ptrs( const sptr &lft, const sptr &rght )
        {
            return lft < rght;
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
            using integer   = derived<type::INTEGER>;
            using floating  = derived<type::FLOAT>;

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
            using boolean   = derived<type::BOOLEAN>;
            using integer   = derived<type::INTEGER>;
            using floating  = derived<type::FLOAT>;
            using string    = derived<type::STRING>;
            using array     = derived<type::ARRAY>;
            using table     = derived<type::TABLE>;

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
                case type::FUNCTION:
                    return compare_ptrs( lft, rght );
                case type::NULL_OBJ:
                    return false;
                case type::RETURN:
                case type::CONT_CALL:
                    break;
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
    class derived<type::TABLE>: public typed_base<type::TABLE> {

        using this_type = derived<type::TABLE>;
    public:
        using sptr = std::shared_ptr<this_type>;

        using value_type = std::map<objects::sptr, objects::sptr, less>;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "{ ";
            for( auto &v: value_ ) {
                oss << v.first->str( ) << ":"
                    << v.second->str( )<< " ";
            }
            oss << "}";
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

        static
        sptr make( )
        {
            return std::make_shared<this_type>( );
        }

    private:
        value_type value_;
    };

    template <>
    class derived<type::ERROR>: public typed_base<type::ERROR> {

        using this_type = derived<type::ERROR>;
    public:
        using sptr = std::shared_ptr<this_type>;

        using value_type = std::string;

        derived( value_type val, tokens::position where )
            :value_(std::move(val))
            ,where_(where)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "error: " << value( ) << "; " << where_;
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

        static
        sptr make( value_type val, tokens::position where )
        {
            return std::make_shared<this_type>( std::move(val), where );
        }

    private:
        value_type value_;
        tokens::position where_;
    };

    using null       = derived<type::NULL_OBJ>;
    using string     = derived<type::STRING>;
    using function   = derived<type::FUNCTION>;
    using cont_call  = derived<type::CONT_CALL>;
    using retutn_obj = derived<type::RETURN>;
    using boolean    = derived<type::BOOLEAN>;
    using integer    = derived<type::INTEGER>;
    using floating   = derived<type::FLOAT>;
    using array      = derived<type::ARRAY>;
    using table      = derived<type::TABLE>;
    using error      = derived<type::ERROR>;

    template <>
    struct type2object<type::TABLE> {
        using native_type = std::map<objects::sptr, objects::sptr, less>;
    };

}}

#endif // OBJECTS_H
