#ifndef MICO_PARSER_H
#define MICO_PARSER_H

#include <map>
#include <memory>

#include "mico/lexer.h"
#include "mico/operations.h"
#include "mico/ast.h"

namespace mico {

    class parser {

    public:

        using expression_uptr = ast::expression::uptr;
        using token_iterator  = lexer::token_list::const_iterator;
        using token_type      = lexer::token_type;
        using token_info      = lexer::token_info;
        using nuds_call       = std::function<expression_uptr( )>;
        using leds_call       = std::function<expression_uptr(expression_uptr)>;

        using nuds_map        = std::map<token_type, nuds_call>;
        using leds_map        = std::map<token_type, leds_call>;

        using errors_list     = std::vector<std::string>;

        parser( lexer lex )
            :lexer_(std::move(lex))
        { }

        void reset( )
        {
            cur_ = peek_ = lexer_.begin( );
            if( peek_ != lexer_.end( ) ) {
                ++peek_;
            }
            errors( ).clear( );
        }

        static
        operations::precedence precedence( token_type tt )
        {
            using TT = token_type;
            using OP = operations::precedence;

            static const std::map<TT, OP> val = {
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

        const errors_list &errors( ) const
        {
            return errors_;
        }

    private:

        lexer           lexer_;
        token_iterator  cur_;
        token_iterator  peek_;
        nuds_map        nuds_;
        leds_map        leds_;

        errors_list     errors_;
    };
}

#endif // PARSER_H
