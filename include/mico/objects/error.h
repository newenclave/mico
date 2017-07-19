#ifndef MICO_ERROR_H
#define MICO_ERROR_H

#include <sstream>
#include <string>

#include "mico/objects/base.h"
#include "mico/tokens.h"
#include "mico/ast.h"

namespace mico { namespace objects {

    template <>
    class impl<type::FAILURE>: public typed_base<type::FAILURE> {

        using this_type = impl<type::FAILURE>;
    public:

        static const type type_value = type::FAILURE;
        using sptr = std::shared_ptr<this_type>;

        using value_type = std::string;

        impl( tokens::position where, value_type val )
            :where_(where)
            ,value_(std::move(val))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "error: [" << where_ << "] " << value( );
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            auto res = make( where_, value_ );
            return res;
        }

    private:

        static
        void out_err( std::ostream & ) { }

        template <typename HeadT, typename ...Rest>
        static
        void out_err( std::ostream &o, const HeadT &h, Rest&&...rest )
        {
            o << h;
            out_err( o, std::forward<Rest>(rest)... );
        }
    public:

        template <typename ...Args>
        static
        objects::sptr make( const ast::node *n, Args&&...args )
        {
            std::ostringstream oss;
            out_err( oss, std::forward<Args>(args)...);
            return make( n->pos( ), oss.str( ) );
        }

        template <typename ...Args>
        static
        objects::sptr make( tokens::position where, Args&&...args )
        {
            std::ostringstream oss;
            out_err( oss, std::forward<Args>(args)...);
            return std::make_shared<this_type>( where, oss.str( ) );
        }

        template <typename ...Args>
        static
        objects::sptr make( Args&&...args )
        {
            std::ostringstream oss;
            out_err( oss, std::forward<Args>(args)... );
            tokens::position pos(0, 0);
            return make(pos, oss.str( ) );
        }

        ast::node::uptr to_ast( tokens::position /*pos*/ ) const override
        {
            return nullptr;
        }

    private:
        tokens::position where_;
        value_type value_;
    };

}}

#endif // ERROR_H
