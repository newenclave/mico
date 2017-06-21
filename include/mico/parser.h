#ifndef MICO_PARSER_H
#define MICO_PARSER_H

#include <map>
#include <memory>

#include "mico/lexer.h"
#include "mico/operations.h"
#include "mico/ast.h"
#include "mico/expressions.h"
#include "mico/statements.h"

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

        using precedence      = operations::precedence;

        parser( lexer lex )
            :lexer_(std::move(lex))
        {
            reset( );
            fill_nuds( );
            fill_leds( );
        }

        void reset( )
        {
            cur_ = peek_ = lexer_.begin( );
            if( peek_ != lexer_.end( ) ) {
                ++peek_;
            }
            errors_.clear( );
        }

        void fill_nuds( )
        {
            nuds_[token_type::IDENT] = [this]( ) { return parse_ident( ); };
        }

        void fill_leds( )
        {
        }

        static
        operations::precedence get_precedence( token_type tt )
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

        ////////////// errors ///////

        void error_expect( token_type tt )
        {
            std::ostringstream oss;

            oss << "parser error: " << peek( ).line << ":" << peek( ).pos
                << " expected token to be " << tt
                <<  ", got "
                << peek( ).ident.name
                << "(" << peek( ).ident.literal << ") instead";
            errors_.emplace_back(oss.str( ));
        }

        void error_no_prefix( )
        {
            std::ostringstream oss;
            oss << "no prefix parse function for "
                   << current( ).ident.name
                   << "(" << current( ).ident.literal << ")"
                   << " found";
            errors_.emplace_back(oss.str( ));
        }

        void error_no_suffix( )
        {
            std::ostringstream oss;
            oss << "no suffix parse function for "
                   << current( ).ident.name
                   << "(" << current( ).ident.literal << ")"
                   << " found";
            errors_.emplace_back(oss.str( ));
        }

        /////////////////////////////

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

        bool is_current( token_type tt ) const
        {
            return current( ).ident.name == tt;
        }

        bool is_peek( token_type tt ) const
        {
            return peek( ).ident.name == tt;
        }

        bool expect_peek( token_type tt, bool error = true )
        {
            if( is_peek( tt ) ) {
                advance( );
                return true;
            } else if( error ) {

            }
            return false;
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

    public: /////////////////// PARSING

        ast::expressions::ident::uptr parse_ident( )
        {
            using ident_type = ast::expressions::ident;
            ident_type::uptr res( new ident_type(current( ).ident.literal ) );
            return res;
        }

        ast::expression::uptr parse_expr( precedence p )
        {
            ast::expression::uptr expr;

            auto nud = nuds_.find( current( ).ident.name );
            if( nud == nuds_.end( ) ) {
                error_no_prefix( );
                return ast::expression::uptr( );
            }
            expr = nud->second( );
            return expr;
        }

        ast::statements::let::uptr parse_let( )
        {
            using let_type = ast::statements::let;

            if( !expect_peek( token_type::IDENT ) ) {
                return let_type::uptr( );
            }

            std::string ident_name = current( ).ident.literal;

            if( !expect_peek( token_type::ASSIGN ) ) {
                return let_type::uptr( );
            }

            advance( );

            ast::expression::uptr nn(parse_expr( precedence::LOWEST ));

            //advance( );

            while( !eof( ) && peek( ).ident.name != token_type::SEMICOLON ) {
                advance( );
            }

            //ast::expression::uptr nn(new ast::expressions::null);

            return let_type::uptr(new let_type(ident_name, std::move(nn) ) );
            //return ast::statements::let::uptr(new ast::statements::let(ident_name));
        }

        ast::program parse( )
        {
            ast::program prog;

            while( !eof( ) ) {

                ast::statement::uptr stmnt;

                switch( current( ).ident.name ) {
                case token_type::LET:
                    stmnt = parse_let( );
                    break;
                default:
                    break;
                }

                if( stmnt ) {
                    prog.push_state( std::move(stmnt) );
                }

                advance( );
            }

            prog.set_errors( errors_ );

            return prog;
        }

        static
        ast::program parse( std::string input )
        {

            auto tt = mico::lexer::make(input);

            ast::program::error_list errors;

            for( auto &e: tt.errors( ) ) {
                errors.emplace_back( std::string("lexer error: ") + e );
            }

            if( !errors.empty( ) ) {
                ast::program prog;
                prog.set_errors( std::move(errors) );
                return prog;
            }

            parser pp( std::move(tt) );

            return pp.parse( );
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
