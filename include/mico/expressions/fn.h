#ifndef MICO_EXPRESSIONS_FN_H
#define MICO_EXPRESSIONS_FN_H

#include <sstream>
#include <map>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::FN>: public typed_expr<type::FN> {

        using this_type = impl<type::FN>;

    public:

        using uptr = std::unique_ptr<impl>;

        using stmt_type   = statement::uptr;
        using ident_type  = expression::uptr;
        using init_map    = std::map<std::string, expression::uptr>;

        using params_slist = std::shared_ptr<expression_list>;
        using body_slist   = std::shared_ptr<statement_list>;

        impl( )
            :params_(std::make_shared<expression_list>( ))
            ,body_(std::make_shared<statement_list>( ))
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
            for( auto &ex: body( ) ) {
                oss << ex->str( ) << ";\n";
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

        body_slist &body_ptr( )
        {
            return body_;
        }

        const expression_list &params( ) const
        {
            return *params_;
        }

        expression_list &params( )
        {
            return *params_;
        }

        const statement_list &body( ) const
        {
            return *body_;
        }

        statement_list &body( )
        {
            return *body_;
        }

        void mutate( mutator_type call ) override
        {
            for( auto &ini: inits_ ) {
                ast::expression::apply_mutator( ini.second, call );
            }
            for( auto &par: *params_ ) {
                ast::expression::apply_mutator( par, call );
            }
            for( auto &bod: *body_ ) {
                ast::statement::apply_mutator( bod, call );
            }
        }

        bool is_const( ) const override
        {
            for( auto &b: *body_ ) {
                if( !b->is_const( ) ) {
                    return false;
                }
            }
            return true;
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
            for( auto &bod: *body_ ) {
                res->body_->emplace_back( ast::statement::call_clone( bod ) );
            }
            return res;
        }

    private:
        init_map      inits_;
        params_slist  params_;
        body_slist    body_;
    };

}}}

#endif // FN_H
