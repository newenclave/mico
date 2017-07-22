#ifndef MICO_EXPRESSION_QUOTE_H
#define MICO_EXPRESSION_QUOTE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::QUOTE>: public typed_expr<type::QUOTE> {

        using this_type = impl<type::QUOTE>;
    public:

        using uptr = std::unique_ptr<this_type>;

        impl<type::QUOTE>( ast::node::uptr val )
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "quote(" << value_->str( ) << ")";
            return oss.str( );
        }

        ast::node::uptr &value( )
        {
            return value_;
        }

        const ast::node::uptr &value( ) const
        {
            return value_;
        }

        static
        uptr make( ast::node::uptr val )
        {
//            if( !val ) {
//                throw std::logic_error( "Nothig to qote!" );
//            }
            return uptr(new this_type( std::move( val ) ) );
        }

        void mutate( mutator_type call ) override
        {
            //// hm
            ast::node::apply_mutator( value_, call );
        }

        ast::node::uptr clone( ) const override
        {
            return make( value_->clone( ) );
        }

        bool is_const( ) const override
        {
            return false;
        }


    private:
        ast::node::uptr value_;
    };

}}}
#endif // QUOTE_H
