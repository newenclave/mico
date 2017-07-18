#ifndef MICO_AST_H
#define MICO_AST_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>

#include "mico/tokens.h"

#ifdef __clang__
#   pragma clang diagnostic ignored "-Wswitch"
#elif defined __GNUC__
#  pragma GCC diagnostic ignored "-Wswitch"
#endif

namespace mico { namespace ast {

    enum class type {
        NONE = 0,
        PROGRAM,
        IDENT,
        LET,
        EXPR,
        RETURN,
        PREFIX,
        INFIX,
        STRING,
        ARRAY,
        INTEGER,
        BOOLEAN,
        FLOAT,
        TABLE,
        FN,
        CALL,
        INDEX,
        IFELSE,
        REGISTRY,
        QUOTE,
        UNQUOTE,
        MACRO,
    };

    struct name {
        static
        const char *get( type t )
        {
            switch (t) {
            case type::NONE    : return "none";
            case type::PROGRAM : return "PROGRAM";
            case type::IDENT   : return "IDENTIFIER";
            case type::LET     : return "LET";
            case type::EXPR    : return "EXPRESSION";
            case type::RETURN  : return "RETURN";
            case type::PREFIX  : return "PREFIX";
            case type::INFIX   : return "INFIX";
            case type::STRING  : return "STRING";
            case type::ARRAY   : return "ARRAY";
            case type::INTEGER : return "INTEGER";
            case type::BOOLEAN : return "BOOLEAN";
            case type::FLOAT   : return "FLOAT";
            case type::TABLE   : return "TABLE";
            case type::FN      : return "FUNCTION";
            case type::CALL    : return "CALL";
            case type::INDEX   : return "INDEX";
            case type::IFELSE  : return "IFELSE";
            case type::REGISTRY: return "REGISTRY";
            case type::QUOTE   : return "QUOTE";
            case type::UNQUOTE : return "UNQUOTE";
            case type::MACRO   : return "MACRO";
            }
            return "<INVALID>";
        }
    };

    struct node {

        using sptr = std::shared_ptr<node>;
        using uptr = std::unique_ptr<node>;

        using reduce_call = std::function<node::uptr(node *)>;

        virtual ~node( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;
        virtual uptr clone( ) const = 0;

        const tokens::position &pos( ) const
        {
            return pos_;
        }

        void set_pos( tokens::position p )
        {
            pos_ = p;
        }

        virtual
        bool is_expression( ) const
        {
            return false;
        }


        virtual
        node::uptr reduce( reduce_call )
        {
            return nullptr;
        }

    private:
        tokens::position pos_;
    };

    template <type TN, typename BaseT>
    struct typed_node: public BaseT {
        type get_type( ) const override
        {
            return TN;
        }
    };

    using node_sptr = std::shared_ptr<node>;
    using node_uptr = std::unique_ptr<node>;

    //////////////// STATEMENS
    class statement: public node {
    public:
        using uptr = std::unique_ptr<statement>;
        using sptr = std::shared_ptr<statement>;

        static
        void call_reduce( uptr &target, node::reduce_call call )
        {
            if( auto nn = target->reduce( call ) ) {
                auto red = cast( nn );
                target.swap( red );
            }
        }

        static
        uptr clone_call( const uptr &n )
        {
            auto cl = n->clone( );
            return cast( cl );
        }

        static
        uptr cast( node::uptr &n )
        {
#if defined(CHECK_CASTS)
            if( n->is_expression( ) ) {
                throw  std::logic_error( "Bad statement cast." );
            }
#endif
            return uptr(static_cast<statement *>(n.release( ) ) );
        }
    };

    template <type TN>
    using typed_stmt = typed_node<TN, statement>;

    ////////////////// EXPRESSIONS
    class expression: public node {
    public:
        using uptr = std::unique_ptr<expression>;
        using sptr = std::shared_ptr<expression>;
        bool is_expression( ) const override
        {
            return true;
        }

        static
        void call_reduce( uptr &target, node::reduce_call call )
        {
            if( auto nn = target->reduce( call ) ) {
                auto red = cast( nn );
                target.swap( red );
            }
        }

        static
        uptr clone_call( const uptr &n )
        {
            auto cl = n->clone( );
            return cast( cl );
        }

        static
        uptr cast( node::uptr &n )
        {
#if defined(CHECK_CASTS)
            if( !n->is_expression( ) ) {
                throw  std::logic_error( "Bad expression cast." );
            }
#endif
            return uptr(static_cast<expression *>(n.release( ) ) );
        }
    };

    template <type TN>
    using typed_expr = typed_node<TN, expression>;

    using expression_list = std::vector<expression::uptr>;
    using statement_list  = std::vector<statement::uptr>;

    using expression_slist = std::vector<expression::sptr>;
    using statement_slist  = std::vector<statement::sptr>;

    class program: public typed_node<type::PROGRAM, node> {

    public:

        using uptr = std::unique_ptr<program>;
        using sptr = std::shared_ptr<program>;

        using state_list = statement_list;
        using error_list = std::vector<std::string>;

        const std::vector<statement::uptr> &states( ) const
        {
            return states_;
        }

        void push_state( statement::uptr val )
        {
            states_.emplace_back( std::move(val) );
        }

        std::string str( ) const
        {
            std::ostringstream oss;

            for( auto &s: states( ) ) {
                oss << s->str( ) << ";\n";
            }

            return oss.str( );
        }

        const error_list &errors( )
        {
            return errors_;
        }

        state_list &states( )
        {
            return states_;
        }

        void set_errors( error_list err )
        {
            errors_ = std::move(err);
        }

        ast::node::uptr clone( ) const override
        {
            uptr inst(new program);
            for( auto &s: states_ ) {
                inst->states_.emplace_back( ast::statement::clone_call( s ) );
            }
            return inst;
        }

    private:
        state_list states_;
        error_list errors_;
    };

    inline
    std::ostream &operator << ( std::ostream &o, const expression::uptr &obj )
    {
        return o << obj->str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, const statement::uptr &obj )
    {
        return o << obj->str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, node *obj )
    {
        return o << obj->str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, type t )
    {
        return o << name::get( t );
    }

}}

#endif // AST_H
