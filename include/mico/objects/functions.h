#ifndef MICO_OBJECTS_FUNCTIONS_H
#define MICO_OBJECTS_FUNCTIONS_H

#include "mico/environment.h"
#include "mico/objects/base.h"
#include "mico/objects/reference.h"

namespace mico { namespace objects {

    template <type TN>
    class env_object: public typed_base<TN> {

    public:

        env_object( std::shared_ptr<environment> e )
            :env_(e)
        { }

        ~env_object( )
        { }

        std::shared_ptr<environment> env( )
        {
            auto l = env_.lock( );
            return l;
        }

        const std::shared_ptr<environment> env( ) const
        {
            auto l = env_.lock( );
            return l;
        }

        bool lock_in( const environment *e ) override
        {
            auto my_env = env( );
            if( my_env ) {
                if( my_env->is_parent( e ) ) {
                    while( my_env.get( ) != e ) {
                        my_env->lock( );
                        my_env = my_env->parent( );
                    }
                } else if( !e->is_parent( my_env.get( ) ) ) {
                    auto par = environment::common_parent( e, my_env.get( ) );
                    if( !e->is_parent( par ) ) {
                        throw std::logic_error( "Not a parent!" );
                    }
                    if( !my_env->is_parent( par ) ) {
                        throw std::logic_error( "Not a parent!" );
                    }
                    lock_in( par );
                }
                return true;
            }
            return false;
        }

        bool unlock_in( const environment *e ) override
        {
            auto my_env = env( );
            if( my_env ) {
                std::size_t ul = 0;
                if( my_env->is_parent( e ) ) {
                    while( my_env.get( ) != e ) {
                        my_env->unlock( );
                        my_env = my_env->parent( );
                        ++ul;
                    }
                } else if( !e->is_parent( my_env.get( ) ) ) {
                    auto par = environment::common_parent( e, my_env.get( ) );
                    if( par ) {
                        if( !e->is_parent( par ) ) {
                            throw std::logic_error( "Not a parent!" );
                        }
                        if( !my_env->is_parent( par ) ) {
                            throw std::logic_error( "Not a parent!" );
                        }
                        unlock_in( par );
                    }
                }
                return true;
            }
            return false;
        }

        std::size_t locked( ) const override
        {
            if( auto p = env( ) ) {
                return p->locked( );
            }
            return 0;
        }

    private:

        std::weak_ptr<environment>  env_;
        const environment          *locked_ = nullptr;
    };

    template <>
    class derived<type::FUNCTION>: public env_object<type::FUNCTION> {
        using this_type = derived<type::FUNCTION>;
    public:
        using sptr = std::shared_ptr<this_type>;

        derived( std::shared_ptr<environment> e,
                  std::shared_ptr<ast::expression_list> par,
                  std::shared_ptr<ast::statement_list> st )
            :env_object(e)
            ,params_(par)
            ,body_(st)
        { }

        ~derived( )
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(" << params_->size( ) << ")";
            return oss.str( );
        }

        ast::expression_list &params( )
        {
            return *params_;
        }

        const ast::expression_list &params( ) const
        {
            return *params_;
        }

        const ast::statement_list &body( ) const
        {
            return *body_;
        }

        ast::statement_list &body( )
        {
            return *body_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( env( ), params_, body_ );
        }

    private:

        std::shared_ptr<ast::expression_list> params_;
        std::shared_ptr<ast::statement_list>  body_;
    };

    template <>
    class derived<type::BUILTIN>: public env_object<type::BUILTIN> {
        using this_type = derived<type::BUILTIN>;
    public:
        using sptr = std::shared_ptr<this_type>;

        derived( std::shared_ptr<environment> e )
            :env_object(e)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "fn(" << static_cast<const void *>(this) << ")";
            return oss.str( );
        }

        virtual
        objects::sptr call( objects::slist &, environment::sptr )
        {
            return derived<type::NULL_OBJ>::make( );
        }

        virtual
        void init( environment::sptr )
        { }

    private:
    };

    template <>
    class derived<type::CONT_CALL>: public env_object<type::CONT_CALL> {

        using this_type = derived<type::CONT_CALL>;
    public:
        using sptr = std::shared_ptr<this_type>;

        derived(objects::sptr obj, objects::slist p, environment::sptr e)
            :env_object(e)
            ,obj_(obj)
            ,params_(std::move(p))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "cc(" << ")";
            return oss.str( );
        }

        objects::sptr value( )
        {
            return obj_;
        }

        objects::slist &params( )
        {
            return params_;
        }

        std::shared_ptr<base> clone( ) const override
        {
            return std::make_shared<this_type>( obj_, params_, env( ) );
        }

    private:
        objects::sptr   obj_;
        objects::slist  params_;
    };

}}

#endif // FUNCTIONS_H
