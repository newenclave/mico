#ifndef MICO_OBJECTS_MACRO_H
#define MICO_OBJECTS_MACRO_H

#include <deque>
#include "mico/objects/base.h"
#include "mico/objects/reference.h"
#include "mico/objects/null.h"

namespace mico { namespace objects {

    template <>
    class derived<type::MACRO>: public typed_base<type::MACRO> {
        using this_type = derived<type::MACRO>;
    public:
        static const type type_value = type::MACRO;
        using sptr = std::shared_ptr<this_type>;

        using param_iterator = ast::expression_list::iterator;
        using ast_type = ast::expressions::macro;

        derived( std::shared_ptr<ast::expression_list> par,
                 std::shared_ptr<ast::statement_list> st )
            :params_(par)
            ,body_(st)
        { }

        ~derived( )
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "macro(" << param_size( ) << ")";
            return oss.str( );
        }

        const ast::statement_list &body( ) const
        {
            return *body_;
        }

        static
        sptr make( std::shared_ptr<environment> e,
                   std::shared_ptr<ast::expression_list> par,
                   std::shared_ptr<ast::statement_list> st )
        {
            return std::make_shared<derived>( e, par, st, start );
        }

        static
        sptr make( std::shared_ptr<environment> e, this_type &other )
        {
            return std::make_shared<derived>( e, other.params_,
                                              other.body_ );
        }


        param_iterator begin( )
        {
            return params_->begin( );
        }

        param_iterator end( )
        {
            return params_->end( );
        }

        std::size_t param_size( ) const
        {
            return params_->size( );
        }

        ast::statement_list &body( )
        {
            return *body_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( params_, body_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            auto res = ast_type::uptr( new ast_type );
            res->set_pos( pos );
            res->body_ptr( )    = body_;
            res->params_ptr( )  = params_;

            return res;
        }

    private:

        std::shared_ptr<ast::expression_list> params_;
        std::shared_ptr<ast::statement_list>  body_;
    };

}}

#endif // MACRO_H
