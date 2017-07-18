#ifndef MICO_EXPRESSIONS_FN_H
#define MICO_EXPRESSIONS_FN_H

#include <sstream>
#include <map>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::FN>: public typed_expr<type::FN> {

        using this_type = detail<type::FN>;

    public:

        using uptr = std::unique_ptr<detail>;

        using stmt_type   = statement::uptr;
        using ident_type  = expression::uptr;
        using init_map    = std::map<std::string, expression::uptr>;

        using params_slist = std::shared_ptr<expression_list>;
        using body_slist   = std::shared_ptr<statement_list>;

        detail( )
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

        node::uptr clone( ) const override
        {
            uptr res(new this_type);

            for( auto &exp: *params_ ) {
                auto clone = exp->clone( );
                expression::uptr cexpt
                        ( static_cast<expression *>(clone.release( ) ) );
                res->params_->emplace_back( std::move(cexpt) );
            }

            for( auto &exp: *body_ ) {
                auto clone = exp->clone( );
                statement::uptr cexpt
                        ( static_cast<statement *>(clone.release( ) ) );
                res->body_->emplace_back( std::move(cexpt) );
            }

            return res;
        }

        ast::node::uptr reduce( ast::node::reduce_call call ) override
        {
            for( auto &p: *params_ ) {
                ast::expression::call_reduce( p, call );
            }

            for( auto &b: *body_ ) {
                ast::statement::call_reduce( b, call );
            }
            return nullptr;
        }

    private:
        init_map      inits_;
        params_slist  params_;
        body_slist    body_;
    };

}}}

#endif // FN_H
