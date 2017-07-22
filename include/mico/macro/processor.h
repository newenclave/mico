#ifndef MICO_MACRO_PROCESSOR_H
#define MICO_MACRO_PROCESSOR_H

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO

#include <map>
#include <vector>

#include "mico/ast.h"
#include "mico/expressions.h"
#include "mico/statements.h"

namespace mico { namespace macro {

    struct processor {

        using error_list = std::vector<std::string>;

        struct scope {

            scope( ) = default;
            scope( scope *p )
                :parent_(p)
            { }

            using value_map  = std::map<std::string, ast::node::uptr>;

            scope *parent( )
            {
                return parent_;
            }

            void set( std::string name, ast::node::uptr value )
            {
                values_[name] = std::move(value);
            }

            ast::node *get( const std::string &name )
            {
                scope *cur = this;
                while( cur ) {
                    auto f = cur->values_.find( name );
                    if( f != cur->values_.end( ) ) {
                        return f->second.get( );
                    }
                    cur = cur->parent( );
                }
                return nullptr;
            }

        private:

            scope *parent_ = nullptr;
            value_map      values_;
        };

        static
        std::string error_param_size( ast::node *n )
        {
            std::ostringstream oss;
            oss << "error: [" << n->pos( ) << "]"
                << " Not enough actual parameters for macro.";
                ;
            return oss.str( );
        }

        static
        ast::node::uptr apply_macro( ast::node *n, scope *s,
                                     error_list *e )
        {
            using     AT  = ast::type;
            namespace AST = ast::statements;
            namespace AEX = ast::expressions;
            using     QT  = AEX::quote;

            auto cn = ast::cast<AEX::call>( n );
            auto mut = [s, e](ast::node *n) {
                return processor::macro_mutator( n, s, e );
            };

            ast::node::apply_mutator( cn->func( ), mut );
            cn->params( )->mutate( mut );

            if( cn->func( )->get_type( ) == AT::MACRO ) {

                scope mscope( s );
                auto mfunc       = ast::cast<AEX::macro>(cn->func( ).get( ));
                auto param_count = cn->param_list( ).size( );
                std::size_t id   = 0;

                for( auto &p: mfunc->params( )->value( ) ) {
                    if( p->get_type( ) == AT::IDENT ) {
                        if( id < param_count ) {

                            auto &val(cn->param_list( )[id++]);
                            mscope.set( p->str( ), QT::make( std::move(val) ) );

                            /// hm...
//                            if( val->get_type( ) != AT::QUOTE ) {
//                                mscope.set( p->str( ),
//                                       AEX::quote::make( std::move(val) ) );
//                            } else {
//                                mscope.set( p->str( ), std::move(val) );
//                            }
                        } else {
                            /// what about error?
                            mscope.set( p->str( ), AEX::null::make( ) );
                            e->emplace_back( error_param_size(n) );
                        }
                    }
                }

                auto new_body = mfunc->body( )->clone( );
                new_body->mutate( [&mscope, e]( ast::node *n ) {
                    return processor::macro_mutator( n, &mscope, e );
                } );

                return new_body;
            }
            return nullptr;
        }

        static
        ast::node::uptr macro_mutator( ast::node *n, scope *s,
                                       error_list *e )
        {
            using     AT  = ast::type;
            namespace AST = ast::statements;
            namespace AEX = ast::expressions;

            if( !n ) {
                return nullptr;
            }

            if( n->get_type( ) == AT::LET ) {
                auto ln = ast::cast<AST::let>( n );
                if( ln->value( )->get_type( ) == AT::MACRO ) {
                    s->set( ln->ident( )->str( ), std::move( ln->value( ) ) );
                    return AEX::null::make( );
                }

            } else if( n->get_type( ) == AT::CALL ) {
                if( auto res = apply_macro( n, s, e ) ) {
                    return res;
                }
            } else if( n->get_type( ) == AT::LIST ) {
                auto ln = ast::cast<AEX::list>( n );
                if( ln->get_role( ) == AEX::list::role::LIST_SCOPE ) {
                    scope sscope(s);
                    ln->mutate( [&sscope, e]( ast::node *n ) {
                        return processor::macro_mutator( n, &sscope, e );
                    } );
                }
            } else if( n->get_type( ) == AT::IDENT ) {
                auto in = ast::cast<AEX::ident>( n );
                if( auto val = s->get( in->value( ) ) ) {
                    return val->clone( );
                }
            }

            auto me = [s, e]( ast::node *n ) {
                return macro_mutator( n, s, e );
            };
            n->mutate( me );

            return nullptr;
        }

        static
        void process( ast::node *node, error_list &errors )
        {
            scope s;
            return process( &s, node, errors );
        }

        static
        void process( scope *s, ast::node *node, error_list &errors )
        {
            node->mutate( [s, &errors]( ast::node *n ) {
                return macro_mutator( n, s, &errors );
            } );
        }
    };

}}

#endif // !defined(DISABLE_MACRO)
#endif // MACRO_PROCESSOR_H
