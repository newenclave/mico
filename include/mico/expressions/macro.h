#ifndef MICO_EXPRESSIONS_MACRO_H
#define MICO_EXPRESSIONS_MACRO_H

#include <sstream>
#include <map>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::MACRO>: public typed_expr<type::MACRO> {

        using this_type = impl<type::MACRO>;

    public:

        using uptr = std::unique_ptr<this_type>;

        using stmt_type    = statement::uptr;
        using ident_type   = expression::uptr;
        using init_map     = std::map<std::string, node::uptr>;

        using body_type    = ast::node::uptr;
        using list_type    = expressions::impl<ast::type::LIST>;
        using params_type  = list_type::uptr;

        impl( )
            :params_(list_type::make_params( ))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "macro(";
            if( params_ ) {
                oss << params_->str( );
            }
            oss << ") {\n";
            if( body_ ) {
                oss << body_->str( ) << "\n";
            }
            oss << "}";
            return oss.str( );
        }

        const params_type &params( ) const
        {
            return params_;
        }

        std::size_t param_size( ) const
        {
            return params_->value( ).size( );
        }

        const body_type &body( ) const
        {
            return body_;
        }

        void set_body( body_type val )
        {
            body_ = std::move(val);
        }

        void set_params( params_type val )
        {
            params_ = std::move(val);
        }

        void mutate( mutator_type call ) override
        {
            list_type::apply_mutator( params_, call );
            ast::node::apply_mutator( body_,   call );
        }

        bool is_const( ) const override
        {
            return body_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type);
            res->params_ = params_->clone_me( );
            res->body_   = ast::node::call_clone( body_ );
            return res;
        }

    private:

        params_type  params_;
        body_type    body_;
    };

}}}

#endif // !defined(DISABLE_MACRO)
#endif // FN_H
