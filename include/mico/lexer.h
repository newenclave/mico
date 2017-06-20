#ifndef MICO_LEXER_H
#define MICO_LEXER_H

#include <vector>

#include "etool/trees/trie/base.h"
#include "mico/tokens.h"
#include "mico/numeric.h"

namespace mico {

    struct lexer {

        using token_type = tokens::type;
        using token_trie = etool::trees::trie::base<char, token_type>;
        using token_info = tokens::info<std::string>;
        using token_list = std::vector<token_info>;

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

            add_token( res, "0b", tokens::type::INT_BIN );
            add_token( res, "0B", tokens::type::INT_BIN );

            add_token( res, "0t", tokens::type::INT_TRE );
            add_token( res, "0T", tokens::type::INT_TRE );

            add_token( res, "0x", tokens::type::INT_HEX );
            add_token( res, "0X", tokens::type::INT_HEX );

            add_token( res, "0",  tokens::type::INT_OCT );

            add_token( res, "\n",   tokens::type::END_OF_LINE );
            add_token( res, "\r\n", tokens::type::END_OF_LINE );
            add_token( res, "\n\r", tokens::type::END_OF_LINE );

            return res;
        }

        template <typename Itr>
        static Itr skip_whitespaces( Itr b, Itr end  )
        {

        }

    public:
        static
        token_list make_token_list( const std::string &input )
        {
            static auto ttrie = make_trie( );

            token_list res;

            return res;
        }

    };

}

#endif // LEXER_H
