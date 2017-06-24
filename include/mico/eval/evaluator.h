#ifndef MICO_EVALUATOR_H
#define MICO_EVALUATOR_H

#include "mico/objects.h"
#include "mico/ast.h"

namespace mico { namespace eval {

    struct base {
        virtual ~base( ) = default;
        virtual objects::sptr eval( ast::node * ) = 0;
    };

}}

#endif // EVALUATOR_H
