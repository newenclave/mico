#ifndef MICO_EXPRESSIONS_MODULE_H
#define MICO_EXPRESSIONS_MODULE_H

#include <sstream>
#include <map>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::MODULE>: public typed_expr<type::MODULE> {

        using this_type = impl<type::MODULE>;

    public:

        using ident_type    = node::uptr;
        using uptr          = std::unique_ptr<impl>;
        using body_type     = ast::node::uptr;
        using parent_list   = ast::expressions::list::uptr;

        impl<type::MODULE>( ident_type n, parent_list parents )
            :name_(std::move(n))
            ,parents_(std::move(parents))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "module " << name_->str( ) << " {\n";
            if( body_ ) {
                oss << body_->str( ) << "\n";
            }
            oss << "}";
            return oss.str( );
        }

        ast::expressions::list::list_type &parents( )
        {
            return parents_->value( );
        }

        const ast::expressions::list::list_type &parents( ) const
        {
            return parents_->value( );
        }

        ident_type& name( )
        {
            return name_;
        }

        const ident_type& name( ) const
        {
            return name_;
        }

        const body_type &body( ) const
        {
            return body_;
        }

        void set_body( body_type val )
        {
            body_ = std::move(val);
        }

        void mutate( mutator_type call ) override
        {
            ast::node::apply_mutator( name_,    call );
            ast::node::apply_mutator( body_,    call );
            ast::expressions::list::apply_mutator( parents_, call );
        }

        bool is_const( ) const override
        {
            return body_->is_const( );
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type( name_->clone( ), parents_->clone_me( ) ) );
            res->body_    = ast::node::call_clone( body_ );
            res->parents_ = parents_->clone_me( );
            return ast::node::uptr( std::move( res ) );
        }

    private:
        ident_type  name_;
        parent_list parents_;
        body_type   body_;
    };

    using mod = impl<type::MODULE>;

}}}

#endif // MODULE_H
