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

        using stmt_type   = statement::uptr;
        using ident_type  = expression::uptr;
        using init_map    = std::map<std::string, expression::uptr>;

        using body_type   = ast::node::uptr;

        using params_slist = std::shared_ptr<expression_list>;

        impl( )
            :params_(std::make_shared<expression_list>( ))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(";
            bool second = false;
            for( auto &id: params( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << id->str( );
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

        params_slist &params_ptr( )
        {
            return params_;
        }

        const expression_list &params( ) const
        {
            return *params_;
        }

        expression_list &params( )
        {
            return *params_;
        }

        const body_type &body( ) const
        {
            return body_;
        }

        void set_body( body_type val )
        {
            body_ = std::move(val);
        }

        void mutate( mutator_type call ) override
        {
            for( auto &ini: inits_ ) {
                ast::expression::apply_mutator( ini.second, call );
            }
            for( auto &par: *params_ ) {
                ast::expression::apply_mutator( par, call );
            }
            ast::node::apply_mutator( body_, call );
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
                                     expression::call_clone( ini.second ) );
            }
            for( auto &par: *params_ ) {
                res->params_->emplace_back( expression::call_clone( par ) );
            }
            res->body_ = ast::node::call_clone( body_ );
            return res;
        }

    private:
        init_map      inits_;
        params_slist  params_;
        body_type     body_;
    };

}}}

#endif // FN_H
