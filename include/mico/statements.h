#ifndef MICO_STATEMENTS_H
#define MICO_STATEMENTS_H

#include "mico/ast.h"
#include "mico/expressions.h"

namespace mico { namespace ast { namespace statements {

    template <type>
    class impl;

    template <>
    class impl<type::LET>: public typed_stmt<type::LET> {
        using this_type = impl<type::LET>;
    public:
        using uptr = std::unique_ptr<impl>;

        using expr_type        = node::uptr;
        using ident_type       = node::uptr;

        impl( ident_type id, expr_type val )
            :ident_(std::move(id))
            ,expr_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "let " << ident( )->str( ) << " = " << value( )->str( );
            return oss.str( );
        }

        const ident_type &ident( ) const
        {
            return ident_;
        }

        ident_type &ident( )
        {
            return ident_;
        }

        const expr_type &value( ) const
        {
            return expr_;
        }

        expr_type &value( )
        {
            return expr_;
        }

        static
        uptr make( ident_type id, expr_type expr )
        {
            return uptr( new this_type( std::move(id), std::move(expr) ) );
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( ident_, call );
            ast::node::apply_mutator( expr_,  call );
        }

        bool is_const( ) const override
        {
            return ident_->is_const( ) && expr_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            return ast::node::uptr(new this_type(
                                       node::call_clone( ident_ ),
                                       node::call_clone( expr_ ) ) );
        }

    private:

        ident_type ident_;
        expr_type  expr_;
    };

    template <>
    class impl<type::RETURN>: public typed_stmt<type::RETURN> {

        using this_type = impl<type::RETURN>;

    public:

        using uptr = std::unique_ptr<impl>;

        impl( node::uptr val )
            :expr_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "return " << value( )->str( );
            return oss.str( );
        }

        const node *value( ) const
        {
            return expr_.get( );
        }

        node *value( )
        {
            return expr_.get( );
        }

        static
        uptr make( node::uptr val  )
        {
            return uptr( new this_type( std::move(val ) ) );
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( expr_, call );
        }

        bool is_const( ) const override
        {
            return expr_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            return ast::node::uptr(new this_type( node::call_clone( expr_ ) ) );
        }

    private:
        node::uptr expr_;
    };

    template <>
    class impl<type::EXPR>: public typed_stmt<type::EXPR> {
        using this_type = impl<type::EXPR>;
    public:
        using uptr = std::unique_ptr<impl>;

        impl( expression::uptr val )
            :expr_(std::move(val))
        { }

        std::string str( ) const override
        {
            return expr_->str( );
        }

        expression::uptr &value( )
        {
            return expr_;
        }

        static
        uptr make( expression::uptr val )
        {
            return uptr( new this_type( std::move(val) ) );
        }

        void mutate( mutator_type call ) override
        {
            ast::expression::apply_mutator( expr_, call );
        }

        bool is_const( ) const override
        {
            return expr_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            return ast::node::uptr(new this_type(
                                       expression::call_clone( expr_ ) ) );
        }

    private:
        expression::uptr expr_;
    };

    using let   = impl<type::LET>;
    using ret   = impl<type::RETURN>;
    using expr  = impl<type::EXPR>;

}}}

#endif // STATEMENTS_H
