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
                    first = false;
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

        node::uptr clone( ) const override
        {
            uptr res(new this_type);
            for( auto &exp: value_ ) {
                auto clone = exp->clone( );
                expression::uptr cexpt
                        ( static_cast<expression *>(clone.release( ) ) );
                res->value( ).emplace_back( std::move(cexpt) );
            }
            return res;
        }

    private:

        value_type value_;
    };

}}}

#endif // ARRAY_H
