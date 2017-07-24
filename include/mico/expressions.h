#ifndef MICO_EXPRESSIONS_H
#define MICO_EXPRESSIONS_H

#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"
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
#include "mico/expressions/quote.h"
#include "mico/expressions/unquote.h"
#include "mico/expressions/macro.h"

namespace mico { namespace ast { namespace expressions {

    using ident     = impl<type::IDENT>;
    using string    = impl<type::STRING>;
    using array     = impl<type::ARRAY>;
    using integer   = impl<type::INTEGER>;
    using floating  = impl<type::FLOAT>;
    using boolean   = impl<type::BOOLEAN>;

    using prefix    = impl<type::PREFIX>;
    using infix     = impl<type::INFIX>;
    using index     = impl<type::INDEX>;

    using function  = impl<type::FN>;
    using call      = impl<type::CALL>;

    using ifelse    = impl<type::IFELSE>;
    using table     = impl<type::TABLE>;
    using null      = impl<type::NONE>;
    using registry  = impl<type::REGISTRY>;
    using list      = impl<type::LIST>;

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
    using quote             = impl<type::QUOTE>;
    using unquote           = impl<type::UNQUOTE>;
    using macro             = impl<type::MACRO>;
#endif

}}}

#endif // EXPRESSIONS_H
