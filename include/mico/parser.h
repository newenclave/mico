#ifndef MICO_PARSER_H
#define MICO_PARSER_H

#include <map>
#include <set>
#include <memory>
#include <functional>
#include <fstream>

#include "mico/lexer.h"
#include "mico/operations.h"
#include "mico/ast.h"
#include "mico/expressions.h"
#include "mico/statements.h"

namespace mico {

    class parser {

        struct special_token {
            bool disabled = false;

            struct scope_disable {

                scope_disable( )                                   = delete;
                scope_disable( const scope_disable& )              = delete;
                scope_disable( scope_disable && )                  = delete;
                scope_disable &operator = ( const scope_disable& ) = delete;
                scope_disable &operator = ( scope_disable && )     = delete;

                scope_disable( special_token *st, bool disable )
                    :state_(st)
                    ,old_(true)
                {
                    if( state_ ) {
                        old_ = state_->disabled;
                        state_->disabled = disable;
                    }
                }

                ~scope_disable( )
                {
                    if( state_ ) {
                        state_->disabled = old_;
                    }
                }

            private:
                special_token *state_;
                bool           old_;
            };
        };

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
        using special_map     = std::map<token_type, special_token>;

        using precedence      = operations::precedence;

        explicit
        parser( lexer lex )
            :lexer_(std::move(lex))
        {
            reset( );
            fill_nuds( );
            fill_leds( );
            fill_special( );
        }

        void reset( )
        {
            cur_ = peek_ = lexer_.begin( );
            if( peek_ != lexer_.end( ) ) {
                ++peek_;
            }
            errors_.clear( );
        }

        void fill_special( )
        {
            special_[token_type::BREAK].disabled    = true;
            special_[token_type::CONTINUE].disabled = true;
            special_[token_type::ELIPSIS].disabled  = true;
        }

        void fill_nuds( )
        {
            nuds_[token_type::IDENT]   = [this]( ) { return parse_ident( );  };
            nuds_[token_type::INFIN]   = [this]( ) { return parse_inf( );    };
            nuds_[token_type::STRING]  = [this]( ) { return parse_string( ); };
            nuds_[token_type::RSTRING] = [this]( ) { return parse_rstring( );};
            nuds_[token_type::FLOAT]   = [this]( ) { return parse_float( );  };
            nuds_[token_type::LPAREN]  = [this]( ) { return parse_paren( );  };

            nuds_[token_type::BOOL_TRUE]  = [this]( ) { return parse_bool( ); };
            nuds_[token_type::BOOL_FALSE] = [this]( ) { return parse_bool( ); };
            nuds_[token_type::IF]         = [this]( ) { return parse_if( );   };
            nuds_[token_type::FOR]        = [this]( ) { return parse_for( );  };

            nuds_[token_type::MOD_MUT] =
                    [this]( ) {
                        return parse_mut( true );
                    };
            nuds_[token_type::MOD_CONST] =
                    [this]( ) {
                        return parse_mut( false );
                    };

            nuds_[token_type::CHARACTER]  =
                    [this]( ) {
                        return parse_char( );
                    };

            nuds_[token_type::FUNCTION]   =
                    [this]( ) {
                        return parse_function( );
                    };
            nuds_[token_type::MODULE]   =
                    [this]( ) {
                        return parse_module( nullptr );
                    };

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
            nuds_[token_type::MACRO]   =
                    [this]( ) {
                        return parse_macro( );
                    };

            nuds_[token_type::QUOTE]   =
                    [this]( ) {
                        return parse_quote( );
                    };

            nuds_[token_type::UNQUOTE]   =
                    [this]( ) {
                        return parse_unquote( );
                    };
#endif

            nuds_[token_type::LBRACE]   =
                    [this]( ) {
                        return parse_table( );
                    };

            nuds_[token_type::LBRACKET] =
                    [this]( ) {
                        return parse_array( );
                    };
            nuds_[token_type::ELIPSIS] =
                    [this]( ) {
                        return parse_elipsis( );
                    };

            nuds_[token_type::MINUS]    =
            nuds_[token_type::BANG]     =
            nuds_[token_type::TILDA]    =
            nuds_[token_type::ASTERISK] =
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

            nuds_[token_type::END_OF_FILE] =
                    [this]( ) {
                        return unexpected_eof( );
                    };
        }

        void fill_leds( )
        {
            using EP = expression_uptr;
            leds_[token_type::DOT]          =
            leds_[token_type::MINUS]        =
            leds_[token_type::PLUS]         =
            leds_[token_type::ASTERISK]     =
            leds_[token_type::ASSIGN]       =
            leds_[token_type::SLASH]        =
            leds_[token_type::PERCENT]      =
            leds_[token_type::LT]           =
            leds_[token_type::GT]           =
            leds_[token_type::LT_EQ]        =
            leds_[token_type::GT_EQ]        =
            leds_[token_type::EQ]           =
            leds_[token_type::NOT_EQ]       =
            leds_[token_type::LOGIC_OR]     =
            leds_[token_type::LOGIC_AND]    =
            leds_[token_type::BIT_OR]       =
            leds_[token_type::BIT_XOR]      =
            leds_[token_type::BIT_AND]      =
            leds_[token_type::SHIFT_LEFT]   =
            leds_[token_type::SHIFT_RIGHT]  =
            leds_[token_type::DOTDOT]       =
            leds_[token_type::OP_IN]        =
                    [this]( EP e ) {
                        return parse_infix(std::move(e));
                    };
            leds_[token_type::LPAREN]   =
                    [this]( EP e ) {
                        return parse_call(std::move(e));
                    };
            leds_[token_type::LBRACKET]   =
                    [this]( EP e ) {
                        return parse_index(std::move(e));
                    };
        }

        static
        operations::precedence get_precedence( token_type tt )
        {
            using TT = token_type;
            using OP = operations::precedence;

            static const std::map<TT, OP> val = {
                { TT::ASSIGN,       OP::ASSIGN      },
                { TT::EQ,           OP::EQUALS      },
                { TT::NOT_EQ,       OP::EQUALS      },
                { TT::LT,           OP::LESSGREATER },
                { TT::GT,           OP::LESSGREATER },
                { TT::LT_EQ,        OP::LESSGREATER },
                { TT::GT_EQ,        OP::LESSGREATER },
                { TT::LOGIC_OR,     OP::LOGIC_OR    },
                { TT::LOGIC_AND,    OP::LOGIC_AND   },
                { TT::BIT_OR,       OP::BIT_OR      },
                { TT::BIT_XOR,      OP::BIT_XOR     },
                { TT::BIT_AND,      OP::BIT_AND     },
                { TT::SHIFT_LEFT,   OP::BIT_SHIFT   },
                { TT::SHIFT_RIGHT,  OP::BIT_SHIFT   },
                { TT::PLUS,         OP::SUM         },
                { TT::MINUS,        OP::SUM         },
                { TT::SLASH,        OP::PRODUCT     },
                { TT::PERCENT,      OP::PRODUCT     },
                { TT::ASTERISK,     OP::PRODUCT     },
                { TT::LPAREN,       OP::CALL        },
                { TT::LBRACKET,     OP::INDEX       },
                { TT::DOT,          OP::DOT         },
                { TT::DOTDOT,       OP::DOTDOT      },
                { TT::OP_IN,        OP::EQUALS      },
            };

            auto f = val.find( tt );
            if( f !=  val.end( ) ){
                return f->second;
            }
            return OP::LOWEST;
        }

        ////////////// errors ///////

        ast::expression::uptr unexpected_eof( )
        {
            error_eof( );
            return nullptr;
        }

        void error_eof( )
        {
            std::ostringstream oss;
            oss << "parser error: " << current( ).where
                << " unexpected EOF";
            errors_.emplace_back(oss.str( ));
        }

        void error_open_file( const std::string &path,
                              const std::string &err )
        {
            std::ostringstream oss;
            oss << "parser error: '" << path
                << "' " << err;
            errors_.emplace_back(oss.str( ));
        }

        void error_character( )
        {
            std::ostringstream oss;
            oss << "parser error: " << current( ).where
                << " bad char value '" << current( ).ident.literal << "'";
            errors_.emplace_back(oss.str( ));
        }

        void error_unexpect( )
        {
            std::ostringstream oss;
            oss << "parser error: " << current( ).where
                << " Unexpected '" << current( ).ident << "'";
            errors_.emplace_back(oss.str( ));
        }

        void error_expect( token_type tt )
        {
            std::ostringstream oss;
            oss << "parser error: " << peek( ).where
                << " expected token to be '" << tt
                <<  "', got '"
                << peek( ).ident
                << "' instead";
            errors_.emplace_back(oss.str( ));
        }

        void error_no_prefix( )
        {
            std::ostringstream oss;
            oss << current( ).where
                << " no prefix parse function for '"
                << current( ).ident
                << "' found";
            errors_.emplace_back(oss.str( ));
        }

        void error_no_suffix( )
        {
            std::ostringstream oss;
            oss << peek( ).where
                << " no suffix parse function for '"
                << peek( ).ident
                << "' found";
            errors_.emplace_back(oss.str( ));
        }

        void error_inval_data( int pos )
        {
            std::ostringstream oss;
            oss << current( ).where << ":" << pos
                << " invalid data format for '"
                << current( ).ident
                << "'"
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
                error_expect( tt );
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

        special_token *get_spec_tok( token_type tt )
        {
            auto f = special_.find( tt );
            if( f != special_.end( ) ) {
                return &f->second;
            }
            return nullptr;
        }

    public: /////////////////// PARSING

        bool failed( ) const
        {
            return !errors_.empty( );
        }

        bool is_current_expression(  ) const
        {
            if( !is_current( token_type::END_OF_FILE ) ) {
                auto nud = nuds_.find( current( ).ident.name );
                return nud != nuds_.end( );
            }
            return false;
        }

        bool is_peek_expression(  ) const
        {
            if( !is_current( token_type::END_OF_FILE ) ) {
                auto nud = nuds_.find( peek( ).ident.name );
                return nud != nuds_.end( );
            }
            return false;
        }

        static
        ast::node::uptr null( )
        {
            return ast::expressions::null::make( );
        }

        ast::expressions::elipsis::uptr parse_elipsis( )
        {
            using elipsis = ast::expressions::elipsis;

            auto st = get_spec_tok( token_type::ELIPSIS );
            if( st && st->disabled ) {
                error_unexpect( );
                return nullptr;
            }

            if( is_peek_expression(  ) ) {
                advance( );
                auto next = parse_expression( precedence::LOWEST );
                return elipsis::uptr( new elipsis(std::move(next) ) );
            } else {
                auto next = ast::expressions::null::make( );
                return elipsis::uptr( new elipsis(std::move(next) ) );
            }
        }

        ast::expressions::list::uptr parse_scope( )
        {
            auto scope = ast::expressions::list::make_scope( );
            scope->set_pos( current( ).where );
            parse_statements( scope->value( ), token_type::RBRACE );
            return scope;
        }

        ast::expressions::list::uptr parse_scope_expr( )
        {
            auto scope = ast::expressions::list::make_scope( );
            scope->set_pos( current( ).where );
            scope->value( ).emplace_back( parse_expression(precedence::LOWEST));
            return scope;
        }

        ast::expressions::forin::uptr parse_for( )
        {
            using TD = special_token::scope_disable;
            using res_type = ast::expressions::forin;

            advance( );

            auto idents = parse_ident_list( );

            if( failed( ) ) {
                return nullptr;
            }

            if( !expect_peek( token_type::OP_IN ) ) {
                return nullptr;
            }

            advance( );
            auto expr = parse_expression_list( );

            TD tdb( get_spec_tok( token_type::BREAK ), false );
            TD tdc( get_spec_tok( token_type::CONTINUE ), false );

            ast::expressions::list::uptr body;

            if( !expect_peek( token_type::LBRACE, false ) ) {
                advance( );
                body = parse_scope_expr( );
            } else {
                advance( );
                body = parse_scope( );
            }

            auto res = res_type::make( );

            res->set_body( std::move(body) );
            res->set_idents( std::move(idents) );
            res->set_expres( std::move(expr) );

            return res;
        }

        ast::expressions::ifelse::node parse_if_node( )
        {
            ast::expressions::ifelse::node res;

            auto cond = parse_expression( precedence::LOWEST );

            if( !cond ) {
                return res;
            }

            if( !expect_peek( token_type::LBRACE, false ) ) {
                advance( );
                res.body = parse_expression( precedence::LOWEST );
                res.cond = std::move(cond);
                return res;
            }
            advance( );

            res.cond = std::move(cond);
            res.body = parse_scope( );

            return res;
        }

        ast::expressions::ifelse::uptr parse_if( )
        {
            using if_type = ast::expressions::ifelse;
            if_type::uptr res(new if_type);
            res->set_pos( current( ).where );

            do {
                advance( );
                auto next = parse_if_node( );
                if( !next.cond ) {
                    return nullptr;
                }
                res->ifs( ).emplace_back(std::move(next));
            } while ( expect_peek( token_type::ELIF, false ) );

            if( expect_peek( token_type::ELSE, false ) ) {
                if( !expect_peek( token_type::LBRACE, false ) ) {
                    advance( );
                    res->alt( ) = parse_scope_expr( );
                } else {
                    advance( );
                    res->alt( ) = parse_scope( );
                }
            }

            return res;
        }

        ast::expressions::prefix::uptr parse_prefix( )
        {
            ast::expressions::prefix::uptr res;

            auto tt = current( ).ident.name;
            auto pos = current( ).where;

            advance( );
            auto expr = parse_expression( precedence::PREFIX );
            if( expr ) {
                res.reset(new ast::expressions::prefix(tt, std::move(expr) ));
                res->set_pos( pos );
            }
            return res;
        }

        ast::expressions::infix::uptr parse_infix( expression_uptr left )
        {
            ast::expressions::infix::uptr res;

            auto tt = current( ).ident.name;
            res.reset(new ast::expressions::infix(tt, std::move(left)));
            res->set_pos( current( ).where );

            auto cp = get_precedence( tt );

            advance( );

            if( cp == precedence::ASSIGN ) {
                cp = precedence::LOWEST;
            }

            auto right = parse_expression( cp );

            if( !right ) {
                return ast::expressions::infix::uptr( );
            }

            res->set_right( std::move(right) );

            return  res;
        }

        ast::expressions::index::uptr parse_index( expression_uptr left )
        {
            using index_type = ast::expressions::index;
            advance( );
            auto expr = parse_expression( precedence::LOWEST );
            if( expr && expect_peek( token_type::RBRACKET ) ) {
                return index_type::uptr(new index_type(std::move(left),
                                                       std::move(expr) ) );
            }
            return nullptr;
        }

        ast::expressions::array::uptr parse_array( )
        {
            using index_type = ast::expressions::array;
            index_type::uptr res(new index_type);
            do {
                if( expect_peek( token_type::RBRACKET, false ) ) {
                    return res;
                }
                advance( );
                auto next = parse_expression( precedence::LOWEST );
                if( !next ) {
                    return nullptr;
                }
                res->value( ).emplace_back( std::move(next) );
            } while( expect_peek( token_type::COMMA, false ) );

            if( !expect_peek(token_type::RBRACKET) ) {
                return nullptr;
            }

            return res;
        }

        ast::expressions::table::uptr parse_table( )
        {
            using table_type = ast::expressions::table;
            auto res = table_type::uptr(new table_type);
            res->set_pos( current( ).where );

            std::int64_t id = 0;

            if( expect_peek( token_type::RBRACE, false ) ) {
                return res;
            } else {
                advance( );
            }

            do {
                auto expr = parse_expression( precedence::LOWEST );
                expression_uptr val;
                if( expect_peek( token_type::COLON, false ) ) {
                    advance( );
                    val = parse_expression( precedence::LOWEST );
                } else {
                    val.reset( new ast::expressions::integer(id++) );
                    val.swap(expr);
                }
                res->value( ).emplace_back( std::make_pair(std::move(expr),
                                                   std::move(val) ) );
                if( expect_peek( token_type::COMMA, false ) ) {
                    if( is_peek( token_type::RBRACE ) ) {
                        break;
                    } else {
                        advance( );
                    }
                } else if( expect_peek( token_type::RBRACE, false ) ) {
                    break;
                } else if( failed( ) ) {
                    return nullptr;
                }
            } while( true );

            return res;
        }

        ast::expression::uptr parse_paren( )
        {
            ///LPAREN
            advance( );
            auto res = parse_expression( precedence::LOWEST );
            if( expect_peek( token_type::RPAREN, true ) ) {
                //advance( );
            }
            if( res ) {
                res->set_pos( current( ).where );
            }
            return res;
        }

        ast::expressions::list::uptr parse_ident_list( )
        {
            using TD = special_token::scope_disable;
            TD tde( get_spec_tok( token_type::ELIPSIS ), false );

            auto res = ast::expressions::list::make_params( );
            while( is_current( token_type::IDENT ) ) {
                res->value( ).emplace_back(parse_ident( ));
                if( expect_peek( token_type::COMMA, false ) ) {
                    advance( );
                } else {
                    break;
                }
            }
            if( is_current( token_type::ELIPSIS ) ) {
                if(auto el = parse_elipsis( )) {
                    res->value( ).emplace_back( std::move(el) );
                } else {
                    return nullptr;
                }
            }
            return res;
        }

        ast::expressions::list::uptr parse_expression_list( )
        {
            auto res = ast::expressions::list::make_params( );
            if( !is_current( token_type::SEMICOLON ) ) do {
                auto next = parse_expression( precedence::LOWEST );
                if( !next ) {
                    return res;
                }
                res->value( ).emplace_back( std::move(next) );
                if( expect_peek( token_type::COMMA, false ) ) {
                    advance( );
                } else {
                    break;
                }
            } while( true );
            return res;
        }

        bool parse_expression_list( std::vector<ast::expression::uptr> &res )
        {
            if( !is_current( token_type::SEMICOLON ) ) do {
                auto next = parse_expression( precedence::LOWEST );
                if( !next ) {
                    return false;
                }
                res.emplace_back( std::move(next) );
                if( expect_peek( token_type::COMMA, false ) ) {
                    advance( );
                } else {
                    break;
                }
            } while( true );
            return true;
        }

        ast::expressions::infinite::uptr parse_inf( )
        {
            return ast::expressions::infinite::make( );
        }

        ast::expression::uptr parse_mut( bool mutablity )
        {
            advance( );

            using mut_type = ast::expressions::mod_mut;
            using const_type = ast::expressions::mod_const;
            auto res = parse_expression( precedence::LOWEST );

            if( mutablity ) {
                return mut_type::make(std::move(res));
            } else {
                return const_type::make(std::move(res));
            }
        }

        ast::expression::uptr parse_ident( )
        {
            using ident_type = ast::expressions::ident;

            auto res = ident_type::make(current( ).ident.literal );

            if( (peek( ).ident.name == token_type::LPAREN) ) {
                advance( );
                auto call = parse_call( std::move(res) );
                if( call ) {
                    call->set_pos( current( ).where );
                    return ast::expression::uptr( std::move(call) );
                } else {
                    return nullptr;
                }
            }
            res->set_pos( current( ).where );
            return ast::expression::uptr( std::move( res ) );
        }

        ast::expressions::character::uptr parse_char( )
        {
            auto res = charset::encoding::from_file( current( ).ident.literal );
            if( res.size( ) != 1 ) {
                error_character( );
                return nullptr;
            }
            return ast::expressions::character::make(res[0]);
        }

        ast::expressions::string::uptr parse_string( )
        {
            using res_type = ast::expressions::string;
            auto res = res_type::make( );

            res->set_pos( current( ).where );

            do {
                res->value( ).insert( res->value( ).end( ),
                                      current( ).ident.literal.begin( ),
                                      current( ).ident.literal.end( ) );
            } while( expect_peek( token_type::STRING, false ) );
            return res;
        }

        ast::expressions::string::uptr parse_rstring( )
        {
            using res_type = ast::expressions::string;
            auto res = res_type::make( true );

            res->set_pos( current( ).where );

            do {
                res->value( ).insert( res->value( ).end( ),
                                      current( ).ident.literal.begin( ),
                                      current( ).ident.literal.end( ) );
            } while( expect_peek( token_type::RSTRING, false ) );
            return res;
        }

        ast::expressions::boolean::uptr parse_bool( )
        {
            using bool_type = ast::expressions::boolean;
            auto cur = is_current(token_type::BOOL_TRUE);
            bool_type::uptr res( new bool_type( cur ) );
            res->set_pos( current( ).where );
            return res;
        }

        ast::statements::break_expr::uptr parse_break( )
        {
            using break_type = ast::statements::break_expr;
            auto st = get_spec_tok( token_type::BREAK );
            if( st && st->disabled ) {
                error_unexpect( );
                return nullptr;
            }
            return break_type::make( );
        }

        ast::statements::cont_expr::uptr parse_continue( )
        {
            using cont_type = ast::statements::cont_expr;
            auto st = get_spec_tok( token_type::CONTINUE );
            if( st && st->disabled ) {
                error_unexpect( );
                return nullptr;
            }
            return cont_type::make( );
        }

        ast::expressions::integer::uptr parse_int( )
        {
            ast::expressions::integer::uptr res;

            auto pos = current( ).where;
            int failed = -1;
            auto num = numeric::parse_int( current( ).ident.literal,
                                           current( ).ident.name, &failed );
            if( failed >= 0 ) {
                error_inval_data(failed);
            } else {
                res.reset(new ast::expressions::integer(num));
                res->set_pos( pos );
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

            res->set_pos( current( ).where );
            return res;
        }

        ast::expression::uptr parse_expression( precedence p )
        {
            ast::expression::uptr left;

            auto nud = nuds_.find( current( ).ident.name );
            if( nud == nuds_.end( ) ) {
                error_no_prefix( );
                return nullptr;
            }

            left = nud->second( );
            if( !left ) {
                return ast::expression::uptr( );
            }

            auto pp = peek_precedence( );
            auto pt = peek( ).ident.name;

            while( (pt != token_type::SEMICOLON) && (p < pp) ) {

                auto led = leds_.find( pt );
                if( led == leds_.end( ) ) {
                    error_no_suffix( );
                    return nullptr;
                }

                advance( );
                left = led->second(std::move(left));
                if( !left ) {
                    return nullptr;
                }

                pp = peek_precedence( );
                pt = peek( ).ident.name;
            }

            return left;
        }

        ast::statement::uptr parse_fn_state( )
        {
            if( expect_peek( token_type::IDENT, false ) ) {
                auto id     = parse_ident( );
                auto macro  = parse_function( );
                return ast::statements::let::make( std::move(id),
                                                   std::move(macro) );
            } else {
                return parse_expr_stmt( );
            }
        }

        ast::statements::let::uptr parse_let( bool mut )
        {
            using let_type = ast::statements::let;

            if( !expect_peek( token_type::IDENT ) ) {
                return nullptr;
            }

            ast::expression::uptr id = parse_ident( );

            if( !expect_peek( token_type::ASSIGN, true ) ) {
                return nullptr;
            }

            advance( );

            if( is_current( tokens::type::MODULE ) ) {
                auto expr = parse_module( id->clone( ) );
                return let_type::uptr(new let_type( std::move(id),
                                                    std::move(expr), mut ) );
            } else {
                auto expr = parse_expression( precedence::LOWEST );
                return let_type::uptr(new let_type( std::move(id),
                                                    std::move(expr), mut ) );
            }
        }

        ast::expressions::mod::uptr parse_module( ast::node::uptr id )
        {
            using TD = special_token::scope_disable;
            using mod_type = ast::expressions::mod;

            if( expect_peek( token_type::IDENT, false ) ) {
                id = parse_ident( );
            } else if( expect_peek( token_type::STRING, false ) ) {
                id = parse_string( );
            } else /*if( !id )*/ {
                id = ast::expressions::ident::make( "" );
            }

            auto parents = ast::expressions::list::make_params( );

            if( expect_peek( token_type::COLON, false ) ) {
                if( !is_peek( token_type::LBRACE ) ) {
                    advance( );
                    parents = parse_expression_list( );
                }
            }

            if( !expect_peek( token_type::LBRACE, true ) ) {
                return nullptr;
            }

            mod_type::uptr res( new mod_type( std::move(id),
                                              std::move(parents) ) );
            advance( );

            TD tdb( get_spec_tok( token_type::BREAK ), true );
            TD tdc( get_spec_tok( token_type::CONTINUE ), true );

            res->set_body( parse_scope( ) );
            return res;
        }

        ast::statements::ret::uptr parse_return( )
        {
            using res_type = ast::statements::ret;
            advance( );
            if( is_current_expression( ) ) {
                auto expr = parse_expression( precedence::LOWEST );
                return res_type::make( std::move(expr ) );
            } else {
                return res_type::make( null( ) );
            }
        }

        ast::expressions::function::uptr parse_function( )
        {
            using TD = special_token::scope_disable;
            using fn_type = ast::expressions::function;

            fn_type::uptr res(new fn_type);
            res->set_pos( current( ).where );

            if( !expect_peek( token_type::LPAREN ) ) {
                return nullptr;
            }

            advance( );
            if( !is_current( token_type::RPAREN ) ) {
                res->set_params( parse_ident_list( ) );
                if( !expect_peek( token_type::RPAREN ) ) {
                    return nullptr;
                }
            }

            TD tdb( get_spec_tok( token_type::BREAK ),    true );
            TD tdc( get_spec_tok( token_type::CONTINUE ), true );

            if( expect_peek( token_type::LBRACE, false ) ) {
                advance( );
                res->set_body( parse_scope( ) );
                return res;
            } else {
                advance( );
                res->set_body( parse_scope_expr( ) );
                return res;
            }
            //return nullptr;
        }

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO

        ast::statement::uptr parse_macro_state(  )
        {
            if( expect_peek( token_type::IDENT, false ) ) {
                auto id     = parse_ident( );
                auto macro  = parse_macro( );
                return ast::statements::let::make( std::move(id),
                                                   std::move(macro) );
            } else {
                return parse_expr_stmt( );
            }
        }

        ast::expressions::macro::uptr parse_macro( )
        {
            using fn_type = ast::expressions::macro;
            fn_type::uptr res(new fn_type);
            res->set_pos( current( ).where );

            if( !expect_peek( token_type::LPAREN ) ) {
                return nullptr;
            }

            advance( );
            if( !is_current( token_type::RPAREN ) ) {
                res->set_params( parse_ident_list( ) );
                if( !expect_peek( token_type::RPAREN ) ) {
                    return fn_type::uptr( );
                }
            }

            if( !expect_peek( token_type::LBRACE ) ) {
                return nullptr;
            }

            advance( );
            res->set_body( parse_scope( ) );

            return res;
        }

        ast::expressions::quote::uptr parse_expr_quote( )
        {
            using quote_type = ast::expressions::quote;

            if( !expect_peek( token_type::LPAREN ) ) {
                return nullptr;
            }
            advance( );

            auto expr = parse_expression( precedence::LOWEST );

            if( !expect_peek( token_type::RPAREN ) ) {
                return nullptr;
            }

            return quote_type::uptr( new quote_type( std::move(expr) ) );
        }

        ast::expressions::quote::uptr parse_scope_quote( )
        {
            using quote_type = ast::expressions::quote;

            if( !expect_peek( token_type::LBRACE ) ) {
                return nullptr;
            }
            advance( );

            auto expr = parse_scope( ); /// parse scope set current to brace

            return quote_type::uptr( new quote_type( std::move(expr) ) );
        }

        ast::expressions::quote::uptr parse_quote( )
        {
            if( peek( ).ident.name == token_type::LPAREN ) {
                return parse_expr_quote( );
            } else if( peek( ).ident.name == token_type::LBRACE ) {
                return parse_scope_quote( );
            } else {
                error_expect( token_type::LBRACE );
            }
            return nullptr;
        }

        ast::expressions::unquote::uptr parse_unquote( )
        {
            using unquote_type = ast::expressions::unquote;
            if( !expect_peek( token_type::LPAREN ) ) {
                return nullptr;
            }
            advance( );

            auto expr = parse_expression( precedence::LOWEST );

            if( !expect_peek( token_type::RPAREN ) ) {
                return nullptr;
            }
            return unquote_type::uptr( new unquote_type( std::move(expr) ) );
        }
#endif

        ast::expressions::call::uptr parse_call( ast::expression::uptr left )
        {
            using call_type = ast::expressions::call;
            call_type::uptr res(new call_type(std::move(left) ) );
            res->set_pos( current( ).where );
            advance( );

            if( !is_current( token_type::RPAREN ) ) {
                res->set_params( parse_expression_list( ) );
                if( !expect_peek( token_type::RPAREN ) ) {
                    return call_type::uptr( );
                }
            }

            return res;
        }

        using load_result = etool::details::result< mico::file_string,
                                                    std::string >;

        static
        bool file_exists( const std::string &path )
        {
            std::ifstream f(path, std::ifstream::binary);
            return f.is_open( );
        }

        static
        load_result load_file( const std::string &path )
        {
            std::ifstream f(path, std::ifstream::binary);

            if( !f.is_open( ) ) {
                return load_result::fail
                        ( std::string("Unable to open file ") + path );
            }

            f.seekg( 0, f.end );
            auto size = f.tellg( );
            f.seekg( 0, f.beg );

            if( !size ) {
                return load_result::fail
                        ( std::string("File is empty ") + path );
            }

            mico::file_string data( static_cast<std::size_t>( size ), '\0' );
            f.read( &data[0], size );
            return load_result::ok( data );
        }

        ast::statement::uptr parse_export( )
        {
            advance( );
            std::string path;
            std::string modname;
            if( current( ).ident.name == token_type::IDENT ) {
                path = current( ).ident.literal;
            } else if( current( ).ident.name == token_type::STRING ) {
                path = current( ).ident.literal;
            }

            if( expect_peek( token_type::TOKEN_AS, false ) ) {
                advance( );
                if( current( ).ident.name == token_type::IDENT ) {
                    modname = current( ).ident.literal;
                } else if( current( ).ident.name == token_type::STRING ) {
                    modname = current( ).ident.literal;
                }
            }

            if( !file_exists( path ) ) {
                path += ".mico";
            }
            auto opened = load_file( path );
            if( !opened ) {
                error_open_file( path, opened.error( ) );
                return nullptr;
            }

            auto res = parse( *opened );
            if( !res.errors( ).empty( ) ) {
                errors_.insert( errors_.end( ),
                                res.errors( ).begin( ), res.errors( ).end( ) );
                return nullptr;
            }

            auto prog = parse( *opened );

            return ast::statement::uptr( new ast::program( std::move(prog) ) );
        }

        ast::statements::expr::uptr parse_expr_stmt(  )
        {
            using expr_type = ast::statements::expr;
            auto expt = parse_expression(precedence::LOWEST);
            if( !expt ) {
                return expr_type::uptr( );
            }
            ast::statements::expr::uptr res(new expr_type(std::move(expt)));
            return res;
        }

        ast::statement::uptr parse_next(  )
        {
            ast::statement::uptr stmt;

            switch( current( ).ident.name ) {
            case token_type::LET:
                stmt = parse_let( false );
                break;
            case token_type::VAR:
                stmt = parse_let( true );
                break;
            case token_type::RETURN:
                stmt = parse_return( );
                break;
            case token_type::EXPORT:
                stmt = parse_export( );
                break;

            case token_type::BREAK:
                stmt = parse_break( );
                break;

            case token_type::CONTINUE:
                stmt = parse_continue( );
                break;

            case token_type::SEMICOLON:
                break;

/*
            case token_type::FUNCTION:
                stmt = parse_fn_state( );
                break;

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
            case token_type::MACRO:
                stmt = parse_macro_state( );
                break;
#endif
*/

            default:
                stmt = parse_expr_stmt( );
                break;
            }
            return stmt;
        }

        void parse_statements( ast::node_list &stmts, token_type stop )
        {
            while( !eof( ) && !is_current( stop ) ) {

                auto stmt = parse_next( );
                if( stmt ) {
                    stmts.emplace_back( std::move(stmt) );
                }

                advance( );
            }
        }

        ast::program parse( )
        {
            ast::program prog;

            parse_statements( prog.states( ), token_type::END_OF_FILE );

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
        special_map     special_;

        errors_list     errors_;
    };
}

#endif // PARSER_H
