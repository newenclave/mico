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
#include "mico/objects/base.h"
#include "mico/objects/quote.h"
#include "mico/objects/module.h"
#include "mico/objects/generator.h"

namespace mico { namespace objects {

#define MICO_DEFINE_CAST_FUNC( CallPrefix, TypeName )               \
    inline                                                          \
    impl<TypeName> *cast_##CallPrefix( base *val )                  \
    {                                                               \
        return objects::cast<TypeName>(val);                        \
    }                                                               \
    inline                                                          \
    std::shared_ptr<impl<TypeName> > cast_##CallPrefix( sptr val )  \
    {                                                               \
        return objects::cast<TypeName>( val );                      \
    }

    MICO_DEFINE_CAST_FUNC( null,        type::NULL_OBJ  )
    MICO_DEFINE_CAST_FUNC( int,         type::INTEGER   )
    MICO_DEFINE_CAST_FUNC( float,       type::FLOAT     )
    MICO_DEFINE_CAST_FUNC( bool,        type::BOOLEAN   )
    MICO_DEFINE_CAST_FUNC( string,      type::STRING    )
    MICO_DEFINE_CAST_FUNC( table,       type::TABLE     )
    MICO_DEFINE_CAST_FUNC( array,       type::ARRAY     )
    MICO_DEFINE_CAST_FUNC( func,        type::FUNCTION  )
    MICO_DEFINE_CAST_FUNC( builtin,     type::BUILTIN   )
    MICO_DEFINE_CAST_FUNC( tail_call,   type::TAIL_CALL )
    MICO_DEFINE_CAST_FUNC( return,      type::RETURN    )
    MICO_DEFINE_CAST_FUNC( error,       type::FAILURE   )
    MICO_DEFINE_CAST_FUNC( ref,         type::REFERENCE )
    MICO_DEFINE_CAST_FUNC( mod,         type::MODULE    )
    MICO_DEFINE_CAST_FUNC( get,         type::GENERATOR )

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
    MICO_DEFINE_CAST_FUNC( quote,       type::QUOTE     )
#endif

#undef MICO_DEFINE_CAST_FUNC

}}

#endif // OBJECTS_H
