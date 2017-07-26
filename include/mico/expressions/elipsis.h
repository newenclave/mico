#ifndef MICO_EXPRESSIONS_ELIPSIS_H
#define MICO_EXPRESSIONS_ELIPSIS_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::ELIPSIS>: public typed_expr<type::ELIPSIS> {

        using this_type = impl<type::ELIPSIS>;

    public:

        using value_type = ast::node::uptr;
        using uptr       = std::unique_ptr<this_type>;

        impl(value_type node)
            :value_(std::move(node))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "..." << value_->str( );
            return oss.str( );
            //return value_;
        }

        bool is_ident( ) const
        {
            return value_->get_type( ) == ast::type::IDENT;
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( value_, call );
        }

        static
        uptr make( value_type val )
        {
            return uptr(new this_type(std::move(val) ) );
        }

        bool is_const( ) const override
        {
            return false;
        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(value_->clone( )));
        }

    private:

        ast::node::uptr value_;
    };

    using elipsis = impl<type::ELIPSIS>;

}}}

#endif // IDENT_H
