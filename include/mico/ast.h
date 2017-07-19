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
            }
            return "<INVALID>";
        }
    };

    struct node {

        virtual ~node( ) = default;

        using uptr         = std::unique_ptr<node>;
        using sptr         = std::shared_ptr<node>;
        using mutator_type = std::function<uptr (node *)>;

        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;
        virtual void mutate( mutator_type ) = 0;
        virtual uptr clone( ) const = 0;

        template <typename ArtT, typename ...Args>
        static
        typename ArtT::uptr make( Args && ... args )
        {
            typename ArtT::uptr res( new ArtT(std::forward<Args>(args)... ) );
            return res;
        }

        template <typename ArtT, typename ...Args>
        static
        typename ArtT::uptr make( tokens::position pos, Args && ... args )
        {
            typename ArtT::uptr res( new ArtT(std::forward<Args>(args)... ) );
            res->set_pos( pos );
            return res;
        }

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
        bool is_const( ) const = 0;

        static
        bool apply_mutator( uptr &target, const node::mutator_type &call )
        {
            if( auto res = call( target.get( ) ) ) {
                target.swap( res );
                return true;
            }
            return false;
        }

        static
        uptr call_clone( const uptr &target )
        {
            return target->clone( );
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
        uptr call_clone( const uptr &target )
        {
            auto res = target->clone( );
            return cast( res );
        }

        static
        bool apply_mutator( uptr &target, const node::mutator_type &call )
        {
            if( auto res = call( target.get( ) ) ) {
                auto new_val = cast( res );
                target.swap( new_val );
                return true;
            }
            return false;
        }

        static
        uptr cast( node::uptr &n )
        {
#if defined(CHECK_CASTS)
            if( n->is_expression( ) ) {
                throw  std::logic_error( "Bad statement cast." );
            }
#endif
            return uptr(static_cast<statement *>( n.release( ) ) );
        }
    };

    template <type TN>
    using typed_stmt = typed_node<TN, statement>;

    ////////////////// EXPRESSIONS
    class expression: public node {
    public:
        using uptr = std::unique_ptr<expression>;
        using sptr = std::shared_ptr<expression>;

        virtual
        bool is_expression( ) const
        {
            return true;
        }

        static
        uptr call_clone( const uptr &target )
        {
            auto res = target->clone( );
            return cast( res );
        }

        static
        bool apply_mutator( uptr &target, const node::mutator_type &call )
        {
            if( auto res = call( target.get( ) ) ) {
                auto new_val = cast( res );
                target.swap( new_val );
                return true;
            }
            return false;
        }

        static
        uptr cast( node::uptr &n )
        {
#if defined(CHECK_CASTS)
            if( !n->is_expression( ) ) {
                throw  std::logic_error( "Bad expression cast." );
            }
#endif
            return uptr( static_cast<expression *>(n.release( ) ) );
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

        std::string str( ) const override
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

        void mutate( mutator_type call ) override
        {
            for( auto &s: states_ ) {
                ast::statement::apply_mutator( s, call );
            }
        }

        bool is_const( ) const override
        {
            if( errors_.empty( ) ) {
                for( auto &st: states_ ) {
                    if( !st->is_const( ) ) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new program);
            res->errors_ = errors_;
            for( auto &st: states_ ) {
                res->states_.emplace_back( statement::call_clone( st ) );
            }
            return res;
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
