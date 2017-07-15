#ifndef MICO_EXPRESSIONS_IDENT_H
#define MICO_EXPRESSIONS_IDENT_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::IDENT>: public typed_expr<type::IDENT> {

        using this_type = detail<type::IDENT>;

    public:

        using uptr = std::unique_ptr<this_type>;

        detail(std::string val)
            :value_(std::move(val))
        { }

        std::string str( ) const override
        {
            return value_;
        }

        const std::string &value( ) const
        {
            return value_;
        }

    private:
        std::string value_;
    };

}}}

#endif // IDENT_H
