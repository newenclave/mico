#ifndef MICO_EXPRESSIONS_REGISTRY_H
#define MICO_EXPRESSIONS_REGISTRY_H

#include <sstream>
#include <memory>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::REGISTRY>: public typed_expr<type::REGISTRY> {
        using this_type = detail<type::REGISTRY>;
        using key_type  = std::uintptr_t;

    public:

        using uptr = std::unique_ptr<this_type>;

        detail( key_type k )
            :key_(k)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << std::hex << "ref(" << key_ << ")";
            return oss.str( );
        }

        static
        uptr make( key_type k )
        {
            uptr res(new this_type(k));
            return res;
        }

        ast::node::uptr clone( ) const override
        {
            return make(key_);
        }

        key_type value( ) const
        {
            return key_;
        }

    private:
        key_type key_;
    };

}}}

#endif // REGISTRY_H
