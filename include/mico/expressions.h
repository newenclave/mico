#ifndef MICO_EXPRESSIONS_H
#define MICO_EXPRESSIONS_H

#include "mico/ast.h"

namespace mico { namespace ast { namespace expressions {


//virtual type get_type( ) const = 0;
//virtual std::string str( ) const = 0;

    class ident: public expression {

    public:

        ident(std::string val)
            :value_(std::move(val))
        { }

        type get_type( ) const
        {
            return type::IDENT;
        }

        std::string str( ) const
        {
            return value_;
        }

        const std::string &value( ) const
        {
            return value_;
        }

    private:
        std::string value_;
    };

    class null: public expression {

    public:

        type get_type( ) const
        {
            return type::NONE;
        }

        std::string str( ) const
        {
            return "null";
        }
    };

}}}

#endif // EXPRESSIONS_H
