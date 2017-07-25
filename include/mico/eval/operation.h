#ifndef MICO_EVAL_OPERATION_H
#define MICO_EVAL_OPERATION_H

#include <functional>

#include "mico/objects/base.h"
#include "mico/ast.h"
#include "mico/ast.h"
#include "mico/tokens.h"

namespace mico { namespace eval { namespace operations {

    using eval_call = std::function<objects::sptr (ast::node *)>;

    template <objects::type T>
    struct operation;
    /*
     *  objects::sptr eval_prefix( tokens::type, objects::sptr );
     *  objects::sptr eval_infix( tokens::type, objects::sptr,
     *                            ast::node *, eval_call );
     *
     *  objects::sptr eval_index( index *idx, objects::sptr obj,
     *                            eval_call ev, environment::sptr env );
     *
    */
}}}

#endif // OPERATION_H
