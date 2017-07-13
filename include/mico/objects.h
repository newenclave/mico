#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include "mico/environment.h"

#include "mico/objects/reference.h"
#include "mico/objects/numbers.h"
#include "mico/objects/null.h"
#include "mico/objects/table.h"
#include "mico/objects/array.h"
#include "mico/objects/functions.h"
#include "mico/objects/string.h"
#include "mico/objects/error.h"
#include "mico/objects/boolean.h"
#include "mico/objects/return.h"

namespace mico { namespace objects {

    using null       = derived<type::NULL_OBJ>;
    using string     = derived<type::STRING>;
    using function   = derived<type::FUNCTION>;
    using builtin    = derived<type::BUILTIN>;
    using cont_call  = derived<type::CONT_CALL>;
    using retutn_obj = derived<type::RETURN>;
    using boolean    = derived<type::BOOLEAN>;
    using integer    = derived<type::INTEGER>;
    using floating   = derived<type::FLOAT>;
    using array      = derived<type::ARRAY>;
    using reference  = derived<type::REFERENCE>;
    using table      = derived<type::TABLE>;
    using error      = derived<type::FAILURE>;

}}

#endif // OBJECTS_H
