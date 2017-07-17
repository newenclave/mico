#ifndef MICO_REPL_H
#define MICO_REPL_H

#include <string>
#include <iostream>
#include "mico/parser.h"
#include "mico/eval/tree_walking.h"
#include "mico/builtin.h"
#include "mico/objects.h"
#include "mico/state.h"
#include "etool/console/colors.h"

namespace mico {

    struct repl {

        static
        std::ostream &logo( std::ostream &o )
        {
            using namespace etool::console::ccout;
            o << "Hello! This is the Monkey programming language.\n";
            o << "Mico implementation.\n";
            o << "See " << blue <<  "https://interpreterbook.com/\n";
            o << yellow;
            o <<  "           __,__           \n"             ;
            o <<  "  .--.  .-\"     \"-.  .--.  \n"           ;
            o <<  " / .. \\/  .-. .-.  \\/ .. \\ \n"          ;
            o <<  "| |  '|  /   Y   \\  |'  | |\n"            ;
            o <<  "| \\   \\  \\ "
                << blue << "0" << yellow
                <<  " | "
                << blue << "0" << yellow
                << " /  /   / |\n"          ;
            o <<  " \\ '- ,\\.-\"\"\"\"\"\"\"-./, -' / \n"    ;
            o <<  "  ''-' /_   ^ ^   _\\ '-''  \n"            ;
            o <<  "      |  \\._   _./  |      \n"            ;
            o <<  "      \\   \\ '~' /   /      \n"           ;
            o <<  "       '._ '-=-' _.'       \n"             ;
            o <<  "          '-----'          \n"             ;
            o << none;
            return o;
        }

        static
        void run( )
        {
            using namespace etool::console::ccout;
            mico::state st;
            builtin::init( st.env( ) );
            std::string data;
            std::cout << logo << ">>> ";
            while( true ) {
                std::string tmp;
                std::getline( std::cin, tmp );
                if( tmp.empty( ) ) {
                    auto prog = mico::parser::parse( data );
                    if( prog.errors( ).empty( ) ) {
                        //std::cout << prog.str( ) << "\n";
                        mico::eval::tree_walking tv;
                        if( prog.states( ).size( ) > 0 ) {
                            st.GC( st.env( ) );
                            auto obj = tv.eval( &prog, st.env( ) );
                            if( obj->get_type( ) != objects::type::NULL_OBJ ) {
                                bool failed =
                                    (obj->get_type( )==objects::type::FAILURE);
                                if( failed ) {
                                    std::cout << red;
                                }
                                std::cout << obj->str( ) << "\n";
                                if( failed ) {
                                    std::cout << none;
                                }
                            }
                        }
                    } else {
                        for( auto &e: prog.errors( ) ) {
                            std::cout << e << "\n";
                        }
                    }
                    data.clear( );
                    std::cout << ">>> ";
                } else if( data.empty( ) && tmp.size( )==1 && tmp[0] == 'q' ) {
                    break;
                } else {
                    if( !data.empty( ) ) {
                        data += "\n";
                    }
                    data += tmp;
                    std::cout << "  > ";
                }
            }
        }
    };
}

#endif // REPL_H
