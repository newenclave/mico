#ifndef MICO_LEXER_H
#define MICO_LEXER_H

#include <vector>
#include <map>
#include <sstream>

#include "etool/trees/trie/base.h"
#include "mico/tokens.h"
#include "mico/numeric.h"
#include "mico/idents.h"

namespace mico {

    struct lexer {

        using token_type  = tokens::type;
        using token_trie  = etool::trees::trie::base<char, token_type>;
        using token_ident = tokens::type_ident;
        using token_info  = tokens::info;
        using token_list  = std::vector<token_info>;
        using error_list  = std::vector<std::string>;

        struct state {
            state( std::string::const_iterator b )
                :line_itr(b)
                ,begin_itr(b)
            { }
            std::size_t line = 1;
            std::string::const_iterator line_itr;
            std::string::const_iterator begin_itr;
        };

    //private:

        static
        void add_token( token_trie &ttrie, std::string value, token_type tt )
        {
            ttrie.set( value, tt );
        }

        static
        void add_token( token_trie &ttrie, int tt_int )
        {
            auto tt = static_cast<token_type>(tt_int);
            std::string name = tokens::name::get( tt );
            add_token( ttrie, name, tt );
        }

        static
        token_trie make_trie(  )
        {
            auto begin = static_cast<int>(tokens::type::FIRST_VISIBLE);
            const auto end = static_cast<int>(tokens::type::LAST_VISIBLE);

            token_trie res;
            for( ++begin; begin != end; ++begin ) {
                add_token( res, begin );
            }

            add_token( res, "0b",   tokens::type::INT_BIN );
            add_token( res, "0B",   tokens::type::INT_BIN );

            add_token( res, "0t",   tokens::type::INT_TER );
            add_token( res, "0T",   tokens::type::INT_TER );

            add_token( res, "0x",   tokens::type::INT_HEX );
            add_token( res, "0X",   tokens::type::INT_HEX );

            add_token( res, "0",    tokens::type::INT_OCT );

            add_token( res, "\"",   tokens::type::STRING );

            add_token( res, "\n",   tokens::type::END_OF_LINE );
            add_token( res, "\r\n", tokens::type::END_OF_LINE );
            add_token( res, "\n\r", tokens::type::END_OF_LINE );

            add_token( res, "//", tokens::type::COMMENT);

            return res;
        }

        template <typename ItrT>
        static ItrT skip_whitespaces( ItrT b, ItrT end, state *lstate  )
        {
            while( b != end ) {
                if( idents::is_newline(*b) ) {
                    lstate->line++;
                    lstate->line_itr = ++b;
                } else if( !idents::is_whitespace(*b) ) {
                    break;
                } else {
                    ++b;
                }
            }
            return b;
        }

        template <typename ItrT>
        static ItrT skip_comment( ItrT b, ItrT end, state */*lstate*/  )
        {
            while( b != end && !idents::is_newline(*b) ) {
                ++b;
            }
            return b;
        }


        template <typename ItrT>
        static
        std::string read_string( ItrT &begin, ItrT end, state *lstate )
        {
            std::string res;
            for( ; (begin != end) && (*begin != '"'); ++begin ) {

                auto next = std::next(begin);

                if( *begin == '\\' && next != end ) {
                    switch (*next) {
                    case 'n':
                        res.push_back('\n');
                        break;
                    case 'r':
                        res.push_back('\r');
                        break;
                    case 't':
                        res.push_back('\t');
                        break;
                    case '\\':
                        res.push_back('\\');
                        break;
                    case '"':
                        res.push_back('"');
                        break;
                    default:
                        res.push_back('\\');
                        res.push_back(*next);
                        break;
                    }
                    begin = next;
                } else {
                    if( *begin == '\n') {
                        lstate->line++;
                        lstate->line_itr = next;
                    }
                    res.push_back( *begin );
                }
            }

            if( begin != end ) {
                ++begin;
            }

            return res;
        }

        template <typename ItrT>
        static
        std::string read_number( ItrT &begin, ItrT end )
        {
            std::string res;

            for( ; begin != end; ++begin ) {
                if( numeric::valid_for_hex( *begin ) ) {
                    res.push_back( *begin );
                } else if( *begin != '_' ) {
                    break;
                }
            }

            return res;
        }

        template <typename ItrT>
        static
        std::string read_ident( ItrT &begin, ItrT end )
        {
            std::string res;

            for( ; begin != end; ++begin ) {
                if( idents::is_ident( *begin ) ) {
                    res.push_back( *begin );
                } else {
                    break;
                }
            }

            return res;
        }

        template <typename ItrT>
        static
        std::string read_float( ItrT &begin, ItrT end, int *found )
        {
            std::string res;
            *found = 0;
            auto chk = &idents::is_digit;

            for( ;begin != end; ++begin ) {
                if( chk( *begin ) ) {
                    res.push_back( *begin );
                } else if( *begin != '_' ) {
                    break;
                }
            }

            if( begin != end && *begin == '.' ) {
                res.push_back( *begin++ );
                *found = 1;
                for( ;begin != end; ++begin ) {
                    if( chk( *begin ) ) {
                        res.push_back( *begin );
                    } else if( *begin != '_' ) {
                        break;
                    }
                }
            }

            if( begin != end && (*begin == 'e' || *begin == 'E') ) {
                res.push_back( *begin++ );
                *found = 1;
                if( begin != end && (*begin == '+' || *begin == '-') ) {
                    res.push_back( *begin++ );
                }
                for( ;begin != end; ++begin ) {
                    if( chk( *begin ) ) {
                        res.push_back( *begin );
                    } else if( *begin != '_' ) {
                        break;
                    }
                }
            }

            return res;
        }

        template <typename ItrT>
        static
        std::pair<token_ident, ItrT> next_noken( ItrT begin, ItrT end,
                                                 token_trie  &ttrie,
                                                 state *lstate )
        {
            using I = token_ident;

            while( begin != end ) {

                auto bb   = begin;
                token_ident value;
                auto next = ttrie.get(begin, end, true);

                value.name = token_type::NONE;
                int ffound = 0;

                if( next ) {
                    value = token_ident(*next);

                    switch( *next ) {
                    case token_type::COMMENT:
                        bb = skip_comment( next.iterator( ), end, lstate );
                        value.literal = "";
                        return std::make_pair( std::move(value), bb );
                    case token_type::END_OF_LINE:
                        lstate->line++;
                        lstate->line_itr = next.iterator( );
                        break;
                    case token_type::INT_BIN:
                    case token_type::INT_TER:
                    case token_type::INT_HEX:
                        bb = next.iterator( );
                        value.literal = read_number( bb, end );
                        return std::make_pair( std::move(value), bb );
                    case token_type::INT_OCT:
                        bb = std::prev(next.iterator( ));
                        value.literal = read_float(bb, end, &ffound);
                        if( ffound != 0 ) {
                            value.name = token_type::FLOAT;
                        }
                        return std::make_pair( std::move(value), bb );
                    case token_type::DOT:
                        bb = std::prev(next.iterator( ));
                        if( idents::is_digit( *next.iterator( ) ) ) {
                            value.name = token_type::FLOAT;
                            value.literal = read_float(bb, end, &ffound );
                            return std::make_pair( std::move(value), bb );
                        } else {
                            return std::make_pair( std::move(value),
                                                   next.iterator( ) );
                        }
                    case token_type::STRING:
                        bb = next.iterator( );
                        value.literal = read_string( bb, end, lstate );
                        return std::make_pair( std::move(value), bb );
                    default:
                        return std::make_pair( std::move(value),
                                               next.iterator( ) );
                        break;
                    }
                    begin = next.iterator( );
                } else if( idents::is_digit( *bb ) ) {
                    value.name = token_type::INT_DEC;
                    value.literal = read_float(bb, end, &ffound );
                    if( ffound != 0 ) {
                        value.name = token_type::FLOAT;
                    }
                    return std::make_pair( std::move(value), bb );

                } else if( idents::is_alfa(*bb) ) {
                    value.name    = token_type::IDENT;
                    value.literal = read_ident( bb, end  );
                    return std::make_pair( std::move(value), bb );
                } else {
                    return std::make_pair( I(token_type::NONE), begin );
                }
                begin = skip_whitespaces( begin, end, lstate );
            }

            return std::make_pair( I(token_type::END_OF_FILE), end);
        }

        template <typename ItrT>
        void push_error( const state &st, ItrT cur )
        {
            std::ostringstream oss;
            oss << st.line << ":" << std::distance(st.line_itr, cur)
                << " Unexpected symbol '" << *cur << "'";

            errors_.emplace_back( oss.str( ) );
        }

    public:

        static
        lexer make( const std::string &input )
        {
            auto ttrie = make_trie( );

            state lex_state(input.begin( ));
            lexer res;

            auto b = input.begin( );

            b = skip_whitespaces( b, input.end( ), &lex_state );

            while( b != input.end( ) ) {

                auto bb = b;
                token_info ti;
                auto line_start = lex_state.line_itr;
                auto current_line = lex_state.line;

                auto nt = next_noken( b, input.end( ), ttrie, &lex_state );

                if( nt.first.name == token_type::END_OF_FILE ) {
                    break;
                } else if ( nt.first.name == token_type::COMMENT ) {
                    /// do nothing
                } else if( nt.first.name == token_type::NONE ) {
                    res.push_error( lex_state, b );
                    nt.second++;
                } else {

                    ti.ident      = std::move(nt.first);
                    ti.where.line = current_line;
                    ti.where.pos  = std::distance( line_start, bb );

                    res.tokens_.emplace_back(std::move(ti));
                }
                b = skip_whitespaces( nt.second, input.end( ), &lex_state );
            }

            token_info ti;
            ti.ident      = token_ident(token_type::END_OF_FILE);
            ti.where.line = lex_state.line;
            ti.where.pos  = std::distance( lex_state.line_itr, b );
            res.tokens_.emplace_back(std::move(ti));

            return res;
        }

        token_list::const_iterator begin( ) const
        {
            return tokens_.begin( );
        }

        token_list::const_iterator end( ) const
        {
            return tokens_.end( );
        }

        const error_list &errors( ) const
        {
            return errors_;
        }

    private:
        token_list  tokens_;
        error_list  errors_;
    };

}

#endif // LEXER_H
