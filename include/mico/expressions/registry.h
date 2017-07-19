#ifndef MICO_EXPRESSION_REGISTRY_H
#define MICO_EXPRESSION_REGISTRY_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::REGISTRY>: public typed_expr<type::REGISTRY> {

    public:

        using value_type = std::uintptr_t;

        detail<type::REGISTRY>( value_type val )
            :value_(val)
        { }

        std::string str( ) const
        {
            std::ostringstream oss;
            oss << std::hex << "reg(" << value( ) << ")";
            return "null";
        }

        value_type value( ) const
        {
            return value_;
        }

        void mutate( mutator_type ) override
        {

        }

    private:

        value_type value_;
    };

}}}

#endif
