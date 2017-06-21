#ifndef MICO_AST_H
#define MICO_AST_H

#include <string>
#include <memory>

namespace mico { namespace ast {

    enum class type {
        NONE = 0,
        LET,
        RETURN,
    };

    struct node {
        virtual ~node( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;
    };

    using node_sptr = std::shared_ptr<node>;
    using node_uptr = std::unique_ptr<node>;

    class statement: public node {
    public:
        using uptr = std::unique_ptr<statement>;
    };

    class expression: public node {
    public:
        using uptr = std::unique_ptr<expression>;
    };

    class program: public node {
    public:
        using uptr = std::unique_ptr<program>;
    };

}}

#endif // AST_H
