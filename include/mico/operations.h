#ifndef MICO_OPERATIONS_H
#define MICO_OPERATIONS_H

#include "mico/tokens.h"

namespace mico { namespace operations {

    enum class precedence {
        LOWEST,
        ASSIGN,      // var = val
        EQUALS,      // ==
        LESSGREATER, // > or <
        SUM,         // +
        PRODUCT,     // *
        PREFIX,      // -X or !X
        CALL,        // myFunction(X)
        INDEX,       // array[index]
    };

}}

#endif // OPERATIONS_H
