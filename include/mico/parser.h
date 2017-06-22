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
            nuds_[token_type::IDENT]  = [this]( ) { return parse_ident( ); };
            nuds_[token_type::STRING] = [this]( ) { return parse_string( ); };
            nuds_[token_type::FLOAT]  = [this]( ) { return parse_float( ); };

            nuds_[token_type::MINUS] =
            nuds_[token_type::BANG]  =
                    [this]( ) {
                        return parse_prefix( );
                    };

            nuds_[token_type::INT_BIN] =
            nuds_[token_type::INT_TER] =
            nuds_[token_type::INT_OCT] =
            nuds_[token_type::INT_DEC] =
            nuds_[token_type::INT_HEX] =
                    [this]( ) {
                        return parse_int( );
                    };
        }

        void fill_leds( )
        {
            using EP = expression_uptr;
            leds_[token_type::MINUS]    =
            leds_[token_type::PLUS]     =
            leds_[token_type::ASTERISK] =
            leds_[token_type::LT]       =
            leds_[token_type::GT]       =
            leds_[token_type::EQ]       =
            leds_[token_type::NOT_EQ]   =
                    [this]( EP e ) {
                        return parse_infix(std::move(e));
                    };

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
            oss << "parser error: " << peek( ).where
                << " expected token to be " << tt
                <<  ", got "
                << peek( ).ident.name
                << "(" << peek( ).ident.literal << ") instead";
            errors_.emplace_back(oss.str( ));
        }

        void error_no_prefix( )
        {
            std::ostringstream oss;
            oss << current( ).where
                << " no prefix parse function for "
                << current( ).ident
                << " found";
            errors_.emplace_back(oss.str( ));
        }

        void error_no_suffix( )
        {
            std::ostringstream oss;
            oss << peek( ).where
                << " no suffix parse function for "
                << peek( ).ident
                << " found";
            errors_.emplace_back(oss.str( ));
        }

        void error_inval_data( int pos )
        {
            std::ostringstream oss;
            oss << current( ).where << ":" << pos
                << " invalid data format for "
                << current( ).ident
                ;
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

        precedence peek_precedence( ) const
        {
            return get_precedence( peek( ).ident.name );
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

        ast::expressions::prefix::uptr parse_prefix( )
        {
            ast::expressions::prefix::uptr res;

            auto tt = current( ).ident.name;
            advance( );

            auto expr = parse_expr( precedence::PREFIX );

            res.reset(new ast::expressions::prefix(tt, std::move(expr) ));

            return res;
        }

        ast::expressions::infix::uptr parse_infix( expression_uptr left )
        {
            ast::expressions::infix::uptr res;

            auto tt = current( ).ident.name;
            res.reset(new ast::expressions::infix(tt, std::move(left)));

            auto cp = get_precedence( tt );

            advance( );
            res->set_right( parse_expr( cp ) );

            return  res;
        }

        ast::expressions::ident::uptr parse_ident( )
        {
            using ident_type = ast::expressions::ident;
            ident_type::uptr res( new ident_type(current( ).ident.literal ) );
            return res;
        }

        ast::expressions::string::uptr parse_string( )
        {
            using res_type = ast::expressions::string;
            res_type::uptr res( new res_type(current( ).ident.literal ) );
            return res;
        }

        ast::expressions::integer::uptr parse_int( )
        {
            ast::expressions::integer::uptr res;

            int failed = -1;
            auto num = numeric::parse_int( current( ).ident.literal,
                                           current( ).ident.name, &failed );
            if( failed >= 0 ) {
                error_inval_data(failed);
            } else {
                res.reset(new ast::expressions::integer(num));
            }

            return res;
        }

        ast::expressions::floating::uptr parse_float( )
        {
            ast::expressions::floating::uptr res;

            auto b = current( ).ident.literal.begin( );
            auto e = current( ).ident.literal.end( );

            double value = numeric::parse_float(b, e);

            if( b != e ) {
                error_inval_data(0);
            } else {
                res.reset(new ast::expressions::floating(value));
            }

            return res;
        }

        ast::expression::uptr parse_expr( precedence p )
        {
            ast::expression::uptr left;

            auto nud = nuds_.find( current( ).ident.name );
            if( nud == nuds_.end( ) ) {
                error_no_prefix( );
                return ast::expression::uptr( );
            }
            left = nud->second( );

            auto pp = peek_precedence( );
            auto pt = peek( ).ident.name;

            while( (pt != token_type::SEMICOLON) && (p < pp) ) {

                auto led = leds_.find( pt );
                if( led == leds_.end( ) ) {
                    error_no_suffix( );
                    return left;
                }

                advance( );
                left = led->second(std::move(left));

                pp = peek_precedence( );
                pt = peek( ).ident.name;
            }

            return left;
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

            /// TODO parse expression!
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
