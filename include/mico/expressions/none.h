#ifndef MICO_EXPRESSION_NONE_H
#define MICO_EXPRESSION_NONE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::NONE>: public typed_expr<type::NONE> {
        using this_type = impl<type::NONE>;
    public:

        std::string str( ) const override
        {
            return "null";
        }
        void mutate( mutator_type ) override
        {

        }

        ast::node::uptr clone( ) const override
        {
            return ast::node::uptr(new this_type);
        }

        bool is_const( ) const override
        {
            return true;
        }

    };

}}}

#endif // NONE_H
