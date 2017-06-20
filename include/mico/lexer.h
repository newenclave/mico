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
        using token_ident = tokens::type_ident<std::string>;
        using token_info  = tokens::info<std::string>;
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

    private:

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

            add_token( res, "0t",   tokens::type::INT_TRE );
            add_token( res, "0T",   tokens::type::INT_TRE );

            add_token( res, "0x",   tokens::type::INT_HEX );
            add_token( res, "0X",   tokens::type::INT_HEX );

            add_token( res, "0",    tokens::type::INT_OCT );

            add_token( res, "\n",   tokens::type::END_OF_LINE );
            add_token( res, "\r\n", tokens::type::END_OF_LINE );
            add_token( res, "\n\r", tokens::type::END_OF_LINE );

            return res;
        }

        template <typename ItrT>
        static ItrT skip_whitespaces( ItrT b, ItrT end, state *lstate  )
        {
            while( (b != end) ) {
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
        static
        std::string parse_number( ItrT &begin, ItrT end, token_type t )
        {
            std::string res;

            for( ; begin != end; ++begin ) {
                if( numeric::valid_for( t, *begin ) ) {
                    res.push_back( *begin );
                } else if( *begin != '_' ) {
                    break;
                }
            }

            return res;
        }

        template <typename ItrT>
        static
        std::string parse_ident( ItrT &begin, ItrT end )
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

                if( next ) {
                    value = token_ident(*next);

                    switch( *next ) {
                    case token_type::END_OF_LINE:
                        lstate->line++;
                        lstate->line_itr = next.iterator( );
                        break;
                    case token_type::INT_BIN:
                    case token_type::INT_TRE:
                    case token_type::INT_OCT:
                    case token_type::INT_HEX:
                        bb = next.iterator( );
                        value.literal = parse_number( bb, end, *next );
                        return std::make_pair( std::move(value), bb );
                    default:
                        return std::make_pair( std::move(value),
                                               next.iterator( ) );
                        break;
                    }
                    begin = next.iterator( );
                } else if( idents::is_digit( *bb ) ) {
                    value.name = token_type::INT_DEC;
                    value.literal = parse_number( bb, end,
                                                  token_type::INT_DEC );
                    return std::make_pair( std::move(value), bb );
                } else if( idents::is_alfa(*bb) ) {
                    value.name     = token_type::IDENT;
                    value.literal = parse_ident( bb, end  );
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
            oss << "Unexpected symbol '" << *cur << "' at line "
                << st.line << ":" << std::distance(st.line_itr, cur);

            errors_.emplace_back( std::move(oss.str( ) ) );
        }

    public:

        static
        lexer make( const std::string &input )
        {
            static auto ttrie = make_trie( );

            state lex_state(input.begin( ));
            lexer res;

            auto b = input.begin( );

            b = skip_whitespaces( b, input.end( ), &lex_state );

            while( b != input.end( ) ) {
                auto bb = b;
                token_info ti;
                auto nt = next_noken( b, input.end( ), ttrie, &lex_state );
                if( nt.first.name == token_type::END_OF_FILE ) {
                    ti.ident = token_ident(token_type::END_OF_FILE);
                    ti.line  = lex_state.line;
                    ti.pos   = input.size( );
                    res.tokens_.emplace_back(std::move(ti));
                } else if( nt.first.name == token_type::NONE ) {
                    res.push_error( lex_state, b );
                    break;
                } else {
                    ti.ident = std::move(nt.first);
                    ti.line  = lex_state.line;
                    ti.pos   = std::distance( lex_state.line_itr, bb );
                    res.tokens_.emplace_back(std::move(ti));
                }
                b = skip_whitespaces( nt.second, input.end( ), &lex_state );
            }

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
