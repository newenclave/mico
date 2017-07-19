#ifndef MICO_EXPRESSION_EIFELSE_H
#define MICO_EXPRESSION_EIFELSE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::IFELSE>: public typed_expr<type::IFELSE> {

        using this_type = impl<type::IFELSE>;
    public:

        using uptr = std::unique_ptr<impl>;

        struct node {
            node( expression::uptr val )
                :cond(std::move(val))
            { }

            node( ) = default;
            node( node && ) = default;
            node &operator = ( node && ) = default;

            node ( node & ) = delete;
            node &operator = ( node & ) = delete;

            expression::uptr cond;
            statement_list   body;
        };

        using if_list = std::vector<node>;

        std::string str( ) const override
        {
            std::ostringstream oss;

            bool first = true;
            for( auto &f: general_ ) {
                oss << (first ? "if " : " elif " )
                    << f.cond->str( ) << " {\n";
                for( auto &c: f.body ) {
                    oss << c->str( ) << ";\n";
                }
                oss << "}";
                first = false;
            }
            if( !alt_.empty( ) ) {
                oss << " else {\n";
                for( auto &a: alt_ ) {
                    oss << a->str( ) << ";\n";
                }
                oss << "}";
            }

            return oss.str( );
        }

        if_list &ifs( )
        {
            return general_;
        }

        statement_list &alt( )
        {
            return alt_;
        }

        void mutate( mutator_type call ) override
        {
            for( auto &g: general_ ) {
                ast::expression::apply_mutator( g.cond, call );
                for( auto &b: g.body ) {
                    ast::statement::apply_mutator( b, call );
                }
            }
            for( auto &a: alt_ ) {
                ast::statement::apply_mutator( a, call );
            }
        }

        bool is_const( ) const override
        {
            for( auto &g: general_ ) {
                if(g.cond->is_const( )) {
                    for( auto &b: g.body ) {
                        if( !b->is_const( ) ) {
                            return false;
                        }
                    }
                } else {
                    return false;
                }
            }
            for( auto &a: alt_ ) {
                if( !a->is_const( ) ) {
                    return false;
                }
            }
            return true;
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type);
            for( auto &g: general_ ) {
                node next;
                next.cond = expression::call_clone( g.cond );
                for( auto &b: g.body ) {
                    next.body.emplace_back( statement::call_clone( b ) );
                }
                res->general_.emplace_back( std::move(next) );
            }
            for( auto &a: alt_ ) {
                res->alt_.emplace_back( statement::call_clone( a ) );
            }
            return res;
        }

    private:
        if_list         general_;
        statement_list  alt_;
    };

}}}

#endif // EIFELSE_H
