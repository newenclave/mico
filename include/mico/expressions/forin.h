#ifndef MICO_EXPRESSION_FOR_H
#define MICO_EXPRESSION_FOR_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::FORIN>: public typed_expr<type::FORIN> {

        using this_type = impl<type::FORIN>;

    public:

        using body_type   = ast::expressions::list;
        using idents_type = ast::expressions::list;
        using exptrs_type = ast::expressions::list;

        using body_value  = body_type::uptr;
        using idents_value = body_type::uptr;
        using expres_value = body_type::uptr;

        using uptr = std::unique_ptr<impl>;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "for " << idents_->str( ) << " in "
                << expres_->str( ) << " { " << body_->str( ) << "}";
            return oss.str( );
        }

        const body_value &body( ) const
        {
            return body_;
        }
        void set_body( body_value val )
        {
            body_ = std::move(val);
        }


        const idents_value &idents( ) const
        {
            return idents_;
        }
        void set_idents( idents_value val )
        {
            idents_ = std::move(val);
        }

        const expres_value &expres( ) const
        {
            return expres_;
        }
        void set_expres( expres_value val )
        {
            expres_ = std::move(val);
        }

        static
        uptr make( )
        {
            return uptr(new this_type);
        }

        void mutate( mutator_type call ) override
        {
            ast::expressions::list::apply_mutator( idents_, call );
            ast::expressions::list::apply_mutator( expres_, call );
            ast::expressions::list::apply_mutator( body_,   call );
        }

        ast::node::uptr clone( ) const override
        {
            auto res = make( );
            res->idents_ = idents_->clone_me( );
            res->expres_ = expres_->clone_me( );
            res->body_   = body_->clone_me( );
            return ast::node::uptr( std::move(res) );
        }

        bool is_const( ) const override
        {
            return false;
        }

    private:
        idents_value idents_;
        expres_value expres_;
        body_value   body_;
    };

    using forin = impl<type::FORIN>;

}}}

#endif
