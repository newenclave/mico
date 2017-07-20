#ifndef MICO_EXPRESSION_SCOPE_H
#define MICO_EXPRESSION_SCOPE_H

#include <sstream>
#include <deque>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::LIST>: public typed_expr<type::LIST> {
        using this_type = impl<type::LIST>;
    public:

        using uptr      = std::unique_ptr<this_type>;
        //using sptr      = std::shared_ptr<this_type>;
        using list_type = statement_list;

        list_type &value( )
        {
            return value_;
        }

        const list_type &value( ) const
        {
            return value_;
        }

        virtual std::string str( ) const override
        {
            std::ostringstream oss;
            bool first = true;
            oss << "{ \n";
            for( auto &v: value_ ) {
                if( !first ) {
                    oss << "\n";
                } else {
                    first = false;
                }
                oss << v->str( );
            }
            oss << "}";
            return oss.str( );
        }

        static
        uptr make( )
        {
            return uptr(new this_type);
        }

        void mutate( node::mutator_type call ) override
        {
            for( auto &v: value_ ) {
                ast::statement::apply_mutator( v, call );
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
                res->value_.emplace_back( ast::statement::call_clone( v ) );
            }
            return res;
        }

    private:
        list_type value_;
    };

}}}

#endif // SCOPE_H
