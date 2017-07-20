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
        using sptr      = std::shared_ptr<this_type>;
        using list_type = node_list;

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

        template<role R>
        static
        uptr make( )
        {
            return uptr( new this_type(R) );
        }

        static
        uptr make_scope( )
        {
            return make<role::LIST_SCOPE>( );
        }

        static
        uptr make_params( )
        {
            return make<role::LIST_PARAMS>( );
        }

        void mutate( node::mutator_type call ) override
        {
            for( auto &v: value_ ) {
                ast::node::apply_mutator( v, call );
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

        uptr clone_me( ) const
        {
            uptr res(new this_type(scope_));
            for( auto &v: value_ ) {
                res->value_.emplace_back( ast::node::call_clone( v ) );
            }
            return res;
        }

        ast::node::uptr clone( ) const override
        {
            return clone_me( );
        }

    private:
        list_type value_;
        role scope_ = role::LIST_SCOPE;
    };

}}}

#endif // SCOPE_H
