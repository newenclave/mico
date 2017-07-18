#ifndef MICO_EXPRESSION_QUOTE_H
#define MICO_EXPRESSION_QUOTE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"
#include "mico/statements.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::QUOTE>: public typed_expr<type::QUOTE> {

        using this_type = detail<type::QUOTE>;

    public:

        using uptr = std::unique_ptr<detail>;
        using value_type = std::shared_ptr<ast::node>;

        explicit detail( ast::node::uptr node )
            :node_(node.release( ))
        { }

        explicit detail( value_type node )
            :node_(node)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;

            oss << "`("  << node_->str( ) << ")";

            return oss.str( );
        }

        value_type value( ) const
        {
            return node_;
        }

        static
        uptr make( ast::node::uptr node )
        {
            if( node->get_type( ) == ast::type::EXPR) {
                auto st = static_cast<ast::statements::expr *>(node.get( ));
                ast::node::uptr ex( st->value( ).release( ) );
                return uptr(new this_type( std::move( ex ) ) );
            }
            return uptr(new this_type( std::move( node ) ) );
        }

//        ast::node::uptr reduce( ast::node::reduce_call call ) override
//        {
//            return node_->clone( );
//            //return call(node_.get( ));
//        }

        ast::node::uptr clone( ) const override
        {
            return uptr(new this_type(node_) );
        }

    private:
        value_type node_;
    };

}}}

#endif // QUOTE_H
