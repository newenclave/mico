#ifndef MICO_OBJECTS_INTERVAL_H
#define MICO_OBJECTS_INTERVAL_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/none.h"
#include "mico/objects/array.h"
#include "mico/objects/string.h"
#include "mico/objects/numbers.h"

namespace mico { namespace objects {

    template<>
    class impl<type::INTERVAL>: public typed_base<type::INTERVAL> {

        using this_type = impl<type::INTERVAL>;

    public:

        impl<type::INTERVAL>( )
        { }

        ~impl<type::INTERVAL>( )
        { }

        virtual
        objects::sptr begin( ) = 0;

        virtual
        objects::sptr end( ) = 0;

    };

    namespace impls {

        template <objects::type NumT>
        struct obj;


    }

}}

#endif // INTERVAL_H
