#ifndef MICO_OBJECTS_MODULE_H
#define MICO_OBJECTS_MODULE_H

#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"
#include "mico/objects/collectable.h"
#include "mico/statements.h"
#include "mico/expressions/list.h"

namespace mico { namespace objects {

    template <>
    class impl<type::MODULE>: public collectable<type::MODULE> {

        using this_type = impl<type::MODULE>;
    public:

        static const type type_value = type::TABLE;
        using sptr          = std::shared_ptr<this_type>;
        using cont          = impl<type::REFERENCE>;
        using cont_sptr     = std::shared_ptr<cont>;
        using parents_list  = std::deque<sptr>;

        impl<type::MODULE>( environment::sptr e, const std::string &name )
            :collectable<type::MODULE>(e)
            ,name_(name)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "module " << name_;
            return oss.str( );
        }

        const std::string &name( ) const
        {
            return name_;
        }

        hash_type hash( ) const override
        {
            return 0;
        }

        bool equal( const base * ) const override
        {
            return false;
        }

        parents_list &parents( )
        {
            return parents_;
        }

        const parents_list &parents( ) const
        {
            return parents_;
        }

        static
        sptr make( environment::sptr env, const std::string &n )
        {
            return std::make_shared<this_type>( env, n );
        }

        std::size_t marked( ) const override
        {
            return 0;
        }

        objects::sptr get( const std::string &name )
        {
            if( auto e = env( ) ) {
                return e->get_parents_only(name);
            }

//            for( auto p: parents_ ) {
//                if( p->name_ == name ) {
//                    return p;
//                }
//                if( auto v = p->get( name ) ) {
//                    return v;
//                }
//            }

            return nullptr;
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<this_type>( env( ), name_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::statements::impl<ast::type::MODULE>;
            using ident    = ast::expressions::impl<ast::type::IDENT>;
            using lst      = ast::expressions::impl<ast::type::LIST>;
            using let      = ast::statements::impl<ast::type::LET>;

            auto parents = ast::expressions::list::make_params( );

            ast_type::uptr res(new ast_type( ident::uptr(new ident(name_) ),
                                              std::move( parents ) ) );
            auto  body  = lst::make_scope( );

            res->set_pos( pos );

            if( auto e = env( ) ) {
                for( auto &p: e->data( ) ) {

                    auto name = ident::uptr( new ident(p.first) );
                    auto ls = let::make( std::move(name),
                                         p.second->to_ast(pos) );

                    body->value( ).emplace_back( std::move(ls) );
                }
            }

            return ast::node::uptr( std::move( res ) );
        }

    private:
        std::string     name_;
        parents_list    parents_;
    };

    using module = impl<type::MODULE>;

}}

#endif // TABLE_H
