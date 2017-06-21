#ifndef MICO_PARSER_H
#define MICO_PARSER_H

#include <map>

#include "mico/lexer.h"
#include "mico/operations.h"

namespace mico {

    class parser {

    public:

        using token_iterator = lexer::token_list::const_iterator;
        using token_type     = lexer::token_type;
        using token_info     = lexer::token_info;

        parser( lexer lex )
            :lexer_(std::move(lex))
        { }

        void reset( )
        {
            cur_ = peek_ = lexer_.begin( );
            if( peek_ != lexer_.end( ) ) {
                ++peek_;
            }
        }

        static
        operations::precedence precedence( token_type tt )
        {
            using TT = token_type;
            using OP = operations::precedence;

            static const std::map<token_type, operations::precedence> val = {
                { TT::EQ,       OP::EQUALS       },
                { TT::NOT_EQ,   OP::EQUALS       },
                { TT::LT,       OP::LESSGREATER  },
                { TT::GT,       OP::LESSGREATER  },
                { TT::PLUS,     OP::SUM          },
                { TT::MINUS,    OP::SUM          },
                { TT::SLASH,    OP::PRODUCT      },
                { TT::ASTERISK, OP::PRODUCT      },
                { TT::LPAREN,   OP::CALL         },
                { TT::LBRACKET, OP::INDEX        },
            };
            auto f = val.find( tt );
            if( f !=  val.end( ) ){
                return f->second;
            }
            return OP::LOWEST;
        }

        static
        const token_info &eof_token( )
        {
            static const token_info eof_value( token_type::END_OF_FILE );
            return eof_value;
        }

        const token_info &current( ) const
        {
            return (cur_ == lexer_.end( )) ? eof_token( ) : *cur_;
        }

        const token_info &peek( ) const
        {
            return (peek_ == lexer_.end( )) ? eof_token( ) : *peek_;
        }

        void advance( )
        {
            cur_ = peek_;
            if( peek_ != lexer_.end( ) ) {
                ++peek_;
            }
        }

        bool eof( ) const
        {
            return (cur_ == lexer_.end( ))
                || (cur_->ident.name == token_type::END_OF_FILE );
        }

    private:

        lexer lexer_;
        token_iterator cur_;
        token_iterator peek_;
    };
}

#endif // PARSER_H
