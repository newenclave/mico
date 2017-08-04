#ifndef MICO_EXPRESSION_INFINITE_H
#define MICO_EXPRESSION_INFINITE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::INFIN>: public typed_expr<type::INFIN> {
        using this_type = impl<type::INFIN>;
    public:

        using uptr = std::unique_ptr<this_type>;

        explicit
        impl<type::INFIN>( bool neg )
            :negative_(neg)
        { }

        std::string str( ) const override
        {
            return "inf";
        }

        void mutate( mutator_type ) override
        {

        }

        bool is_negative( ) const
        {
            return negative_;
        }

        static
        uptr make( )
        {
            return make(false);
        }

        static
        uptr make( bool neg )
        {
            return uptr(new this_type(neg));
        }

        ast::node::uptr clone( ) const override
        {
            return ast::node::uptr( make(negative_) );
        }

        bool is_const( ) const override
        {
            return true;
        }
    private:
        bool negative_ = false;
    };

    using infinite = impl<type::INFIN>;

}}}

#endif
