#ifndef MICO_OPERATIONS_H
#define MICO_OPERATIONS_H

#include "mico/tokens.h"

namespace mico { namespace operations {

    /// http://en.cppreference.com/w/cpp/language/operator_precedence
    enum class precedence {
        LOWEST,
        DOTDOT,      // dot dot
        ASSIGN,      // var = val
        LOGIC_OR,    // ||
        LOGIC_AND,   // &&
        BIT_OR,      // |
        BIT_XOR,     // &
        BIT_AND,     // ^
        EQUALS,      // ==
        LESSGREATER, // > or <
        BIT_SHIFT,   // >> or <<
        SUM,         // +
        PRODUCT,     // *
        PREFIX,      // -X or !X
        CALL,        // myFunction(X)
        INDEX,       // array[index]
        DOT,         // module.name
    };

}}

#endif // OPERATIONS_H
