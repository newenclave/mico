#ifndef MICO_AST_H
#define MICO_AST_H

#include <string>
#include <memory>

namespace mico { namespace ast {

    enum class type {
        NONE = 0,

    };

    struct node {
        virtual ~node( ) = default;
        virtual std::string str( ) const = 0;
    };

    using node_sptr = std::shared_ptr<node>;
    using node_uptr = std::unique_ptr<node>;

    class statement: public node {

    };

    class expression: public node {

    };

    class program: public node {

    };

}}

#endif // AST_H
