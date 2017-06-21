#ifndef MICO_STATEMENTS_H
#define MICO_STATEMENTS_H

#include "mico/ast.h"
#include "mico/expressions.h"

namespace mico { namespace ast { namespace statements {

    class let: public statement {

    public:
        using uptr = std::unique_ptr<let>;

        let( std::string ival, expression::uptr ex )
            :ident_(std::move(ival))
            ,expr_(std::move(ex))
        { }

        type get_type( ) const
        {
            return type::LET;
        }

        std::string str( ) const
        {
            std::ostringstream oss;
            oss << "let " << ident_ << " = "
                << (expr_ ? expr_->str( ) : "null");
            return oss.str( );
        }

        const std::string &ident( ) const
        {
            return ident_;
        }

        const expression *expr( ) const
        {
            return expr_.get( );
        }

    private:

        std::string      ident_;
        expression::uptr expr_;
    };
}}}

#endif // STATEMENTS_H
