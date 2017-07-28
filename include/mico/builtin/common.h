#ifndef MICO_BUILTIN_COMMON_H
#define MICO_BUILTIN_COMMON_H

#include "mico/parser.h"
#include "mico/objects.h"
#include "mico/state.h"
#include "mico/environment.h"

namespace mico { namespace builtin {
    class common: public objects::builtin {

        static
        void def_init( environment::sptr )
        { }

        objects::sptr call( objects::slist &params,
                            environment::sptr e) override
        {
            return call_( params, e );
        }

        void init( environment::sptr e ) override
        {
            return init_( e );
        }

    public:

        using call_type = std::function< objects::sptr
                                        (objects::slist &, environment::sptr)>;
        using init_type = std::function<void (environment::sptr)>;

        common( environment::sptr e, call_type c )
            :objects::builtin(e)
            ,init_(&common::def_init)
            ,call_(std::move(c))
        { }

        common( environment::sptr e, init_type i, call_type c )
            :objects::builtin(e)
            ,init_(i ? std::move(i) : &common::def_init)
            ,call_(std::move(c))
        { }

        std::uintptr_t stub_number( ) const override
        {
            return reinterpret_cast<std::uintptr_t>(this);
        }

        static
        objects::sptr make( environment::sptr e, call_type c )
        {
            return std::make_shared<common>( e, std::move(c) );
        }

        static
        objects::sptr make( environment::sptr e, init_type i, call_type c )
        {
            return std::make_shared<common>( e, std::move(i), std::move(c) );
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<common>( env( ), init_, call_ );
        }

    private:
        init_type init_;
        call_type call_;
    };
}}


#endif // COMMON_H
