#ifndef MICO_EVALUATOR_H
#define MICO_EVALUATOR_H

#include "mico/objects/objects.h"
#include "mico/ast.h"
#include "mico/environment.h"

namespace mico { namespace eval {

    struct base {
        virtual ~base( ) = default;
        virtual objects::sptr eval( ast::node *, environment::sptr ) = 0;
    };

}}

#endif // EVALUATOR_H
