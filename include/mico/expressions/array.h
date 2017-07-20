#ifndef MICO_EXPRESSIONS_ARRAY_H
#define MICO_EXPRESSIONS_ARRAY_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::ARRAY>: public typed_expr<type::ARRAY> {
        using this_type = impl<type::ARRAY>;
    public:

        using uptr = std::unique_ptr<this_type>;
        using value_type = ast::node_list;

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
                ast::node::apply_mutator( a, call );
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

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type);
            for( auto &v: value_ ) {
                res->value_.emplace_back( node::call_clone( v ) );
            }
            return res;
        }

    private:

        value_type value_;
    };

}}}

#endif // ARRAY_H
