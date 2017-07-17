#ifndef MICO_EXPRESSION_CALL_H
#define MICO_EXPRESSION_CALL_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::CALL>: public typed_expr<type::CALL> {

        using this_type = detail<type::CALL>;
    public:

        using uptr = std::unique_ptr<detail>;
        using param_type = expression::uptr;

        detail( expression::uptr f )
            :expr_(std::move(f))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << expr_->str( ) << "(";
            bool second = false;
            for( auto &par: params( ) ) {
                if( second) {
                    oss << ", ";
                } else {
                    second = true;
                }
                oss << par->str( );
            }
            oss << ")";
            return oss.str( );
        }

        const expression_list &params( ) const
        {
            return params_;
        }

        expression_list &params( )
        {
            return params_;
        }

        expression *func( )
        {
            return expr_.get( );
        }

        const expression *func( ) const
        {
            return expr_.get( );
        }

        node::uptr clone( ) const override
        {
            auto clone = expr_->clone( );
            expression::uptr cexpt
                    ( static_cast<expression *>(clone.release( ) ) );
            uptr res(new this_type( std::move(cexpt) ));

            for( auto &exp: params( ) ) {
                auto clone = exp->clone( );
                expression::uptr cexpt
                        ( static_cast<expression *>(clone.release( ) ) );
                res->params( ).emplace_back( std::move(cexpt) );
            }

            return res;
        }


    private:
        expression::uptr expr_;
        expression_list  params_;
    };


}}}

#endif // CALL_H
