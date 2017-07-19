#ifndef MICO_EXPRESSIONS_ARRAY_H
#define MICO_EXPRESSIONS_ARRAY_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::ARRAY>: public typed_expr<type::ARRAY> {
        using this_type = detail<type::ARRAY>;
    public:

        using uptr = std::unique_ptr<this_type>;
        using value_type = std::vector<expression::uptr>;

        std::string str( ) const override
        {
            std::stringstream oss;

            bool first = true;

            oss << "[";
            for( auto &e: value( ) ) {
                if( !first ) {
                    oss << ", ";
                } else {
                    first = true;
                }
                oss << e->str( );
            }
            oss << "]";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        void mutate( mutator_type call ) override
        {
            for( auto &a: value_ ) {
                ast::expression::apply_mutator( a, call );
            }
        }

        bool is_const( ) const override
        {
            for( auto &v: value_ ) {
                if( !v->is_const( ) ) {
                    return false;
                }
            }
            return true;
        }

    private:

        value_type value_;
    };

}}}

#endif // ARRAY_H
