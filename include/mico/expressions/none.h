#ifndef MICO_EXPRESSION_NONE_H
#define MICO_EXPRESSION_NONE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::NONE>: public typed_expr<type::NONE> {
    public:
        std::string str( ) const
        {
            return "null";
        }
    };

}}}

#endif // NONE_H
