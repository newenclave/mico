#ifndef MICO_AST_H
#define MICO_AST_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>

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
        INTEGER,
        BOOLEAN,
        FLOAT,
        FN,
        CALL,
        IFELSE,
    };

    struct node {
        virtual ~node( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;
    };

    using node_sptr = std::shared_ptr<node>;
    using node_uptr = std::unique_ptr<node>;

    //////////////// STATEMENS
    class statement: public node {
    public:
        using uptr = std::unique_ptr<statement>;
    };

    ////////////////// EXPRESSIONS
    class expression: public node {
    public:
        using uptr = std::unique_ptr<expression>;
    };

    using expression_list = std::vector<expression::uptr>;
    using statement_list  = std::vector<statement::uptr>;

    class program: public node {
    public:

        using uptr = std::unique_ptr<program>;
        using state_list = statement_list;
        using error_list = std::vector<std::string>;

        type get_type( ) const
        {
            return type::PROGRAM;
        }

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

    private:
        state_list states_;
        error_list errors_;
    };

}}

#endif // AST_H
