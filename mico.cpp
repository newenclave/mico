#include <iostream>
#include <fstream>

#include "mico/tokens.h"
#include "mico/lexer.h"

#include "mico/objects.h"
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "mico/repl.h"
#include "mico/charset/encoding.h"

#include <stdio.h>
#include <thread>

#include "etool/details/result.h"

int run_repl( )
{
    mico::repl::run( );

    return 0;
}

using namespace mico;

int run_file( std::string path )
{
    std::ifstream f(path, std::ifstream::binary);
    if( !f.is_open( ) ) {
        std::cerr << "Unable to open file " << path << "\n";
        return 1;
    }

    f.seekg( 0, f.end );
    auto size = f.tellg( );
    f.seekg( 0, f.beg );

    if( !size ) {
        std::cerr << "file is empty " << path << "\n";
        return 2;
    }

    mico::file_string data( size, '\0' );
    f.read( &data[0], size );

    eval::tree_walking tv;
    mico::state st;

    auto ev = [&tv, &st]( ast::node *n ) {
        return tv.eval( n, st.env( ) );
    };

    all::init( st, ev );
    auto prog = parser::parse( data );

    if( prog.errors( ).empty( ) ) {
        macro::processor::process( &st.macros( ), &prog,
                                   prog.errors( ), ev );
    }

    if( prog.errors( ).empty( ) ) {

        auto obj = tv.eval( &prog, st.env( ) );

        if( obj->get_type( ) == objects::type::INTEGER ) {
            auto res = objects::cast_int( obj );
            return static_cast<int>(res->value( ));
        } else if( obj->get_type( ) == objects::type::FAILURE ) {
            std::cerr << obj->str( ) << "\n";
            return 3;
        }

    } else {
        for( auto &e: prog.errors( ) ) {
            std::cerr << e << "\n";
        }
    }
    return 0;
}

int main_lex( );

int main( int argc, char * argv[ ]  )
{
    try {
        if( argc > 1 ) {
            return run_file( argv[1] );
        } else {
            mico::charset::encoding::init_console( );
            return run_repl( );
        }
    } catch ( const std::exception &ex ) {
        std::cerr << "Something wrong: " << ex.what( ) << "\n";
        return 10;
    }
}
