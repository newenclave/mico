#ifndef MICO_LEXER_H
#define MICO_LEXER_H

#include <vector>
#include <map>

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
        static ItrT skip_whitespaces( ItrT b, ItrT end,
                                      ItrT &line_begin,
                                      std::size_t *line  )
        {
            while( (b != end) ) {
                if( idents::is_newline(*b) ) {
                    ++(*line);
                    ++b;
                    line_begin = b;
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
                                                ItrT &line_begin,
                                                std::size_t *line )
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
                        ++(*line);
                        line_begin = next.iterator( );
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
                    // inval
                    return std::make_pair( I(token_type::NONE), begin );
                }
                begin = skip_whitespaces( begin, end, line_begin, line );
            }

            return std::make_pair( I(token_type::END_OF_FILE), end);
        }

    public:

        static
        token_list make_token_list( const std::string &input )
        {
            static auto ttrie = make_trie( );

            std::size_t curren_line = 1;
            auto        curren_line_itr = input.begin( );
            token_list res;

            auto b = input.begin( );

            b = skip_whitespaces( b, input.end( ), curren_line_itr,
                                  &curren_line );

            while( b != input.end( ) ) {
                auto bb = b;
                auto nt = next_noken( b, input.end( ), ttrie,
                                      curren_line_itr,
                                      &curren_line );
                if( nt.first.name == token_type::END_OF_FILE ) {
                    /// end
                } else if( nt.first.name == token_type::NONE ) {
                    /// invalid
                    std::cout << "Invalid token: @" << curren_line
                              << " pos: " << std::distance(curren_line_itr, nt.second)
                              << " val: '" << *nt.second << "'"
                              << "\n";
                    break;
                } else {
                    token_info ti;
                    ti.ident = std::move(nt.first);
                    ti.line  = curren_line;
                    ti.pos   = std::distance(curren_line_itr, bb);

                    std::cout << "token: @" << ti.line
                              << " pos: " << ti.pos
                              << " val: '" << ti.ident.literal << "'"
                              << " num: " << static_cast<int>(ti.ident.name)
                              << "\n";
                }
                b = skip_whitespaces( nt.second, input.end( ), curren_line_itr,
                                     &curren_line );
            }

            return res;
        }

    };

}

#endif // LEXER_H
