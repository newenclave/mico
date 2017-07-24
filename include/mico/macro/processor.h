#ifndef MICO_MACRO_PROCESSOR_H
#define MICO_MACRO_PROCESSOR_H

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO

#include <map>
#include <set>
#include <vector>

#include "mico/ast.h"
#include "mico/expressions.h"
#include "mico/statements.h"

namespace mico { namespace macro {

    struct processor {

        using error_list = std::vector<std::string>;

        struct scope;

        class built_in_macro: public ast::typed_expr<ast::type::BUILTIN_MACRO> {

            using this_type = built_in_macro;

        public:

            using uptr = std::unique_ptr<this_type>;

            std::string str( ) const override
            {
                std::ostringstream oss;
                oss << "macro(<builtin>)";
                return oss.str( );
            }

            void mutate( mutator_type /*call*/ ) override
            { }

            bool is_const( ) const override
            {
                return false;
            }

            virtual
            ast::node::uptr call( ast::node *, scope *, ast::node_list &,
                                  error_list *) = 0;

        };

        struct scope {

            scope( ) = default;
            scope( scope *p )
                :parent_(p)
            { }

            using value_map     = std::map<std::string, ast::node::uptr>;
            using built_in_map  = std::map<std::string, built_in_macro::uptr>;
            using remap_set     = std::set<std::string>;

            scope *parent( )
            {
                return parent_;
            }

            void set( std::string name, ast::node::uptr value )
            {
                values_[name] = std::move(value);
            }

            void set_bi( std::string name, built_in_macro::uptr value )
            {
                built_in_[name] = std::move(value);
            }

            void deny( std::string name )
            {
                remaped_.insert(name);
            }

            built_in_macro *get_bi( const std::string &name )
            {
                scope *cur = this;
                while( cur ) {
                    auto rf = remaped_.find( name );
                    if( rf != remaped_.end( ) ) {
                        return nullptr;
                    }
                    auto f = cur->built_in_.find( name );
                    if( f != cur->built_in_.end( ) ) {
                        return f->second.get( );
                    }
                    cur = cur->parent( );
                }
                return nullptr;
            }

            ast::node *get( const std::string &name )
            {
                scope *cur = this;
                while( cur ) {
                    auto rf = remaped_.find( name );
                    if( rf != remaped_.end( ) ) {
                        return nullptr;
                    }
                    auto f = cur->values_.find( name );
                    if( f != cur->values_.end( ) ) {
                        return f->second.get( );
                    }
                    cur = cur->parent( );
                }
                return nullptr;
            }

            const value_map &value( ) const
            {
                return values_;
            }

            const built_in_map &built_in( ) const
            {
                return built_in_;
            }

        private:

            scope *parent_ = nullptr;
            value_map      values_;
            built_in_map   built_in_;
            remap_set      remaped_;
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
        ast::node::uptr unlist( ast::node::uptr lst )
        {
            if( lst->get_type( ) == ast::type::LIST ) {
                auto l = ast::cast<ast::expressions::list>(lst.get( ));
                if( l->value( ).size( ) == 1 ) {
                    return std::move(l->value( )[0]);
                }
            }
            return lst;
        }

        static
        ast::node::uptr apply_statement_expr( ast::node *n, scope *s,
                                              error_list *e )
        {
            auto stm = ast::cast<ast::statements::expr>( n );
            auto mut = macro_mutator( stm->value( ).get( ), s, e );
            return mut ? std::move(mut) : std::move(stm->value( ));
        }

        static
        ast::node::uptr apply_macro( ast::node *n, scope *s,
                                     error_list *e )
        {
            using     AT  = ast::type;
            namespace AST = ast::statements;
            namespace AEX = ast::expressions;

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
                            ast::node::apply_mutator( val, mut );

//                            mscope.set( p->str( ), QT::make( std::move(val) ) );

                            /// hm...
                            if( val->get_type( ) != AT::QUOTE ) {
                                mscope.set( p->str( ),
                                       AEX::quote::make( std::move(val) ) );
                            } else {
                                mscope.set( p->str( ), std::move(val) );
                            }
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

                return unlist(std::move(new_body));

            } else if( cn->func( )->get_type( ) == AT::BUILTIN_MACRO ) {
                scope mscope( s );
                auto mfunc = ast::cast<built_in_macro>(cn->func( ).get( ));
                ast::node_list params;
                for( auto &param: cn->param_list( ) ) {
                    params.emplace_back( std::move(param) );
                }
                return mfunc->call( n, &mscope, params, e );
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

            auto me = [s, e]( ast::node *n ) {
                return macro_mutator( n, s, e );
            };

            if( n->get_type( ) == AT::LET ) {
                auto ln = ast::cast<AST::let>( n );
                if( ln->value( )->get_type( ) == AT::MACRO ) {
                    s->set( ln->ident( )->str( ), std::move( ln->value( ) ) );
                    return AEX::null::make( );
                } else {
                    //s->deny( ln->ident( )->str( ) );
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
            } else if( n->get_type( ) == AT::EXPR ) {
                return apply_statement_expr( n, s, e );
            } else if( n->get_type( ) == AT::IDENT ) {
                auto in = ast::cast<AEX::ident>( n );

                if( auto val = s->get( in->value( ) ) ) {
                    return val->clone( );
                }

                /// built in macroses have less priority
                if( auto val = s->get_bi( in->value( ) ) ) {
                    return val->clone( );
                }

            }
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
