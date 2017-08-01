#ifndef MICO_TYPES_H
#define MICO_TYPES_H

#include "mico/charset/encoding.h"

namespace mico {
    using string      = charset::internal_string;
    using sys_string  = charset::sys_string;
    using con_string  = charset::con_string;
    using file_string = charset::file_string;
}

#endif // TYPES_H
