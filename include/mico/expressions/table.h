#ifndef MICO_EXPRESSION_TABLE_H
#define MICO_EXPRESSION_TABLE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::TABLE>: public typed_expr<type::TABLE> {
        using this_type = detail<type::TABLE>;
    public:

        using value_pair = std::pair<expression::uptr, expression::uptr>;
        using value_type = std::vector<value_pair>;
        using uptr = std::unique_ptr<this_type>;

        std::string str( ) const override
        {
            std::ostringstream oss;

            bool first = true;
            for( auto &v: value_ ) {
                if( first ) {
                    first = false;
                } else {
                    oss << ", ";
                }
                oss << v.first->str( ) << ":" << v.second->str( );
            }

            return oss.str( );
        }

        value_type &value( )
        {
            return value_;
        }

        const value_type &value( ) const
        {
            return value_;
        }

        void mutate( mutator_type call ) override
        {
            for( auto &v: value_ ) {
                ast::expression::apply_mutator( v.first,  call );
                ast::expression::apply_mutator( v.second, call );
            }
        }

        bool is_const( ) const override
        {
            for( auto &v: value_ ) {
                if( !v.first->is_const( ) || !v.second->is_const( ) ) {
                    return false;
                }
            }
            return true;
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type);
            for( auto &v: value_ ) {
                res->value_.emplace_back( expression::call_clone( v.first ),
                                          expression::call_clone( v.second ) );
            }
            return res;
        }

    private:

        value_type value_;
    };

}}}

#endif // TABLE_H
