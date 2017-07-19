#ifndef MICO_EXPRESSIONS_H
#define MICO_EXPRESSIONS_H

#include "mico/expressions/detail.h"
#include "mico/expressions/fn.h"
#include "mico/expressions/value.h"
#include "mico/expressions/array.h"
#include "mico/expressions/ident.h"
#include "mico/expressions/string.h"
#include "mico/expressions/prefix.h"
#include "mico/expressions/infix.h"
#include "mico/expressions/none.h"
#include "mico/expressions/index.h"
#include "mico/expressions/call.h"
#include "mico/expressions/ifelse.h"
#include "mico/expressions/table.h"
#include "mico/expressions/registry.h"

namespace mico { namespace ast { namespace expressions {

    using ident     = detail<type::IDENT>;
    using string    = detail<type::STRING>;
    using array     = detail<type::ARRAY>;
    using integer   = detail<type::INTEGER>;
    using floating  = detail<type::FLOAT>;
    using boolean   = detail<type::BOOLEAN>;

    using prefix    = detail<type::PREFIX>;
    using infix     = detail<type::INFIX>;
    using index     = detail<type::INDEX>;

    using function  = detail<type::FN>;
    using call      = detail<type::CALL>;

    using ifelse    = detail<type::IFELSE>;
    using table     = detail<type::TABLE>;
    using null      = detail<type::NONE>;
    using registry  = detail<type::REGISTRY>;
}}}

#endif // EXPRESSIONS_H
