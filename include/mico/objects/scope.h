#ifndef MICO_OBJECTS_SCOPE_H
#define MICO_OBJECTS_SCOPE_H

#include "mico/objects/base.h"
#include "mico/ast.h"

namespace mico { namespace objects {
    template <>
    class derived<type::SCOPE>: public typed_base<type::SCOPE> {

        using this_type = derived<type::SCOPE>;

    public:

        using value_type = std::shared_ptr<ast::statement_list>;
        using sptr = std::shared_ptr<this_type>;

        derived<type::SCOPE>( )
            :body_(std::make_shared<ast::statement_list>( ))
        { }

        derived<type::SCOPE>( value_type val )
            :body_(val)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;

            oss << "{";
            oss << "...";
            oss << "}";

            return oss.str( );
        }

        static
        sptr make( )
        {
            return std::make_shared<this_type>( );
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<this_type>( val );
        }

        objects::sptr clone( ) const override
        {
            auto res = make( );
            res->body_ = body_;
            return res;
        }

        value_type &value_ptr( )
        {
            return body_;
        }

        ast::statement_list &value( )
        {
            return *body_;
        }

        const ast::statement_list &value( ) const
        {
            return *body_;
        }

    public:

        value_type body_;

    };
}}

#endif // SCOPE_H
