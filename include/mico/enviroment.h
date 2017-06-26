#ifndef MICO_ENVIROMENT_H
#define MICO_ENVIROMENT_H

#include <mico/objects.h>

namespace mico {

    class enviroment {

    public:

        using sptr = std::shared_ptr<enviroment>;

        enviroment( ) = default;
        enviroment( sptr env )
            :parent_(env)
        { }

        void set( const std::string &name, objects::sptr val )
        {
            data_[name] = val;
        }

        objects::sptr get( const std::string &name )
        {
            auto cur = this;
            objects::sptr res;
            while( cur && !res ) {
                auto f = data_.find( name );
                if( f != data_.end( ) ) {
                    res = f->second;
                } else {
                    cur = cur->parent_.get( );
                }
            }
            return res;
        }

    private:

        sptr parent_;
        std::map<std::string, objects::sptr> data_;
    };
}

#endif // ENVIROMENT_H
