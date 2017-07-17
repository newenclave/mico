#ifndef MICO_EXPRESSIONS_FN_H
#define MICO_EXPRESSIONS_FN_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::FN>: public typed_expr<type::FN> {

        using this_type = detail<type::FN>;

    public:

        using uptr = std::unique_ptr<detail>;

        using stmt_type  = statement::uptr;
        using ident_type = expression::uptr;

        detail( )
            :ident_(std::make_shared<expression_list>( ))
            ,expr_(std::make_shared<statement_list>( ))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(";
            bool second = false;
            for( auto &id: idents( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << id->str( );
            }
            oss << ") {\n";
            for( auto &ex: states( ) ) {
                oss << ex->str( ) << ";\n";
            }
            oss << "}";
            return oss.str( );
        }

        std::shared_ptr<expression_list> ident_ptr( )
        {
            return ident_;
        }

        std::shared_ptr<statement_list> expr_ptr( )
        {
            return expr_;
        }

        const expression_list &idents( ) const
        {
            return *ident_;
        }

        expression_list &idents( )
        {
            return *ident_;
        }

        const statement_list &states( ) const
        {
            return *expr_;
        }

        statement_list &states( )
        {
            return *expr_;
        }

        node::uptr clone( ) const override
        {
            uptr res(new this_type);

            for( auto &exp: *ident_ ) {
                auto clone = exp->clone( );
                expression::uptr cexpt
                        ( static_cast<expression *>(clone.release( ) ) );
                res->ident_->emplace_back( std::move(cexpt) );
            }

            for( auto &exp: *expr_ ) {
                auto clone = exp->clone( );
                statement::uptr cexpt
                        ( static_cast<statement *>(clone.release( ) ) );
                res->expr_->emplace_back( std::move(cexpt) );
            }

            return res;
        }

    private:

        std::shared_ptr<expression_list> ident_;
        std::shared_ptr<statement_list> expr_;
    };

}}}

#endif // FN_H
