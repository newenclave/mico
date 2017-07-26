#ifndef MICO_EXPRESSIONS_FN_H
#define MICO_EXPRESSIONS_FN_H

#include <sstream>
#include <map>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::FN>: public typed_expr<type::FN> {

        using this_type = impl<type::FN>;

    public:

        using uptr = std::unique_ptr<impl>;

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
            oss << "fn(";
            if( params_ ) {
                oss << params_->str( );
            }
            oss << ") {\n";
            for( auto &init: inits( ) ) {
                oss << "let " << init.first
                    << " = " << init.second->str( ) << ";\n";
            }
            if( body_ ) {
                oss << body_->str( ) << "\n";
            }
            oss << "}";
            return oss.str( );
        }

        init_map &inits( )
        {
            return inits_;
        }

        const init_map &inits( ) const
        {
            return inits_;
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
            for( auto &ini: inits_ ) {
                ast::node::apply_mutator( ini.second, call );
            }
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
            for( auto &ini: inits_ ) {
                res->inits_.emplace( ini.first,
                                     node::call_clone( ini.second ) );
            }
            res->params_ = params_->clone_me( );
            res->body_   = ast::node::call_clone( body_ );
            return ast::node::uptr( std::move( res ) );
        }

    private:
        init_map     inits_;
        params_type  params_;
        body_type    body_;
    };

    using function = impl<type::FN>;

}}}

#endif // FN_H
