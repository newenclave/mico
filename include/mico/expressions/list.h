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

        enum class role {
            LIST_SCOPE,
            LIST_PARAMS,
        };

    private:
        static
        const char *role2str( role r)
        {
            switch (r) {
            case role::LIST_SCOPE:  return ";\n";
            case role::LIST_PARAMS: return ", ";
            }
            return " ";
        }

    public:

        using uptr      = std::unique_ptr<this_type>;
        //using sptr      = std::shared_ptr<this_type>;
        using list_type = statement_list;

        impl<type::LIST>( role scope )
            :scope_(scope)
        { }

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
            for( auto &v: value_ ) {
                if( !first ) {
                    oss << role2str( scope_ );
                } else {
                    first = false;
                }
                oss << v->str( );
            }
            return oss.str( );
        }

        static
        uptr make_scope( )
        {
            return uptr(new this_type(role::LIST_SCOPE));
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
            uptr res(new this_type(scope_));
            for( auto &v: value_ ) {
                res->value_.emplace_back( ast::statement::call_clone( v ) );
            }
            return res;
        }

    private:
        list_type value_;
        role scope_ = role::LIST_SCOPE;
    };

}}}

#endif // SCOPE_H
