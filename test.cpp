#include "mico/eval/tree_walking.h"

#include "mico/lexer.h"
#include "etool/slices/container.h"

using namespace mico;

struct type_ident {

    using value_type = std::string;
    using slice = etool::slices::container<value_type::const_iterator>;

    type_ident( ) = default;
    type_ident( const type_ident & ) = default;
    type_ident& operator = ( const type_ident & ) = default;

    type_ident( tokens::type tt )
        :name(tt)
    { }

    type_ident( tokens::type tt, slice val )
        :literal(std::move(val))
        ,name(tt)
    { }

    type_ident( type_ident &&other )
        :literal(std::move(other.literal))
        ,name(other.name)
    { }

    type_ident& operator = ( type_ident &&other )
    {
        name     = other.name;
        literal = std::move(other.literal);
        return *this;
    }

    std::string lit( ) const
    {
        return literal.empty( )
             ? tokens::name::get( name )
             : std::string(literal.begin( ), literal.end( ));
    }

    slice        literal;
    tokens::type name;
};

struct info2 {

    using value_type = std::string;
    using slice = etool::slices::container<value_type::const_iterator>;

    info2( ) = default;
    info2( const info2 & ) = default;
    info2& operator = ( const info2 & ) = default;


    info2( tokens::type t )
        :ident(t)
    { }

    info2( tokens::type t, slice value )
        :ident(t, std::move(value))
    { }

    info2( info2 &&other )
        :where(other.where)
        ,ident(std::move(other.ident))
    { }

    info2& operator = ( info2 &&other )
    {
        where = other.where;
        ident = std::move(other.ident);
        return *this;
    }

    tokens::position where;
    type_ident       ident;
};

class lexer2 {


public:

    using token_type  = tokens::type;
    using token_ident = type_ident;
    using value_type = std::string;
    using slice = etool::slices::container<value_type::const_iterator>;

    lexer2( )
        :trie_(lexer::make_trie( ))
    { }

    struct position_state {
        position_state(  ) = default;
        position_state( std::string::const_iterator b )
            :line_itr(b)
            ,begin_itr(b)
        { }
        std::size_t line = 1;
        std::string::const_iterator line_itr;
        std::string::const_iterator begin_itr;
    };

    const info2 &current( ) const
    {
        return current_;
    }

    const info2 &peek( ) const
    {
        return peek_;
    }

    const position_state &state( ) const
    {
        return lstate_;
    }

    std::size_t pos( ) const
    {
        return std::distance( lstate_.line_itr, iter_ );
    }

    void advance( )
    {
        if( !eof( ) ) {
            current_ = std::move(peek_);
            peek_    = next( );
        }
    }

    bool eof( ) const
    {
        return current( ).ident.name == token_type::END_OF_FILE;
    }

    void reset( std::string newinput )
    {
        input_   = std::move( newinput );
        iter_    = input_.begin( );
        lstate_  = position_state(iter_);

        current_ = next( );
        peek_    = next( );
    }

    template <typename ItrT, typename EItrT = ItrT >
    static ItrT skip_whitespaces( ItrT b, EItrT end, position_state *lstate  )
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
    static ItrT skip_comment( ItrT b, ItrT end, position_state */*lstate*/  )
    {
        while( b != end && !idents::is_newline(*b) ) {
            ++b;
        }
        return b;
    }

    template <typename ItrT>
    static
    slice read_string( ItrT &begin, ItrT end, position_state *lstate )
    {
        slice::iterator res = begin;
        for( ; (begin != end) && (*begin != '"'); ++begin ) {

            auto next = std::next(begin);

            if( *begin == '\\' ) {
                if( next != end && *next == '"' ) {
                    begin = next;
                }
            } else if( *begin == '\n') {
                lstate->line++;
                lstate->line_itr = next;
            }
        }

        slice r = slice(res, begin);

        if( begin != end ) {
            ++begin;
        }

        return r;
    }

    template <typename ItrT>
    static
    slice read_number( ItrT &begin, ItrT end )
    {
        slice::iterator res = begin;

        for( ; begin != end; ++begin ) {
            if( !numeric::valid_for_hex_( *begin ) ) {
                break;
            }
        }

        return slice(res, begin);
    }

    template <typename ItrT>
    static
    slice read_ident( ItrT &begin, ItrT end )
    {
        slice::iterator res = begin;

        for( ; begin != end; ++begin ) {
            if( !idents::is_ident( *begin ) ) {
                break;
            }
        }

        return slice(res, begin);
    }

    template <typename ItrT>
    static
    slice read_float( ItrT &begin, ItrT end, int *found )
    {
        slice::iterator res = begin;
        *found = 0;
        auto chk = &idents::is_digit;

        for( ;begin != end; ++begin ) {
            if( chk( *begin ) ) {
            } else if( *begin != '_' ) {
                break;
            }
        }

        if( begin != end && *begin == '.' ) {
            *found = 1;
            for( ;begin != end; ++begin ) {
                if( chk( *begin ) ) {
                } else if( *begin != '_' ) {
                    break;
                }
            }
        }

        if( begin != end && (*begin == 'e' || *begin == 'E') ) {
            *found = 1;
            ++begin;
            if( begin != end && (*begin == '+' || *begin == '-') ) {
                ++begin;
            }
            for( ;begin != end; ++begin ) {
                if( chk( *begin ) ) {
                } else if( *begin != '_' ) {
                    break;
                }
            }
        }

        return slice(res, begin);
    }

    template <typename ItrT>
    static
    std::pair<token_ident, ItrT> next_noken( ItrT begin, ItrT end,
                                             lexer::token_trie  &ttrie,
                                             position_state *lstate )
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
                    value.literal = slice( );
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
                return std::make_pair( I(token_type::INVALID), ++begin );
            }
            begin = skip_whitespaces( begin, end, lstate );
        }

        return std::make_pair( I(token_type::END_OF_FILE), end);
    }

    info2 next( )
    {
        slice::iterator b = skip_whitespaces( iter_, input_.end( ), &lstate_);

        while( b != input_.end( ) ) {

            auto bb = b;
            info2 ti;
            auto line_start =  lstate_.line_itr;
            auto current_line = lstate_.line;

            auto nt = next_noken( b, input_.cend( ), trie_, &lstate_ );

            if( nt.first.name == token_type::END_OF_FILE ) {
                //break;
            } else if ( nt.first.name == token_type::COMMENT ) {
                continue;
            } else {

                ti.ident      = std::move(nt.first);
                ti.where.line = current_line;
                ti.where.pos  = std::distance( line_start, bb );
                iter_         = nt.second;

                return ti;
            }
            //b = skip_whitespaces( nt.second, input.end( ), &lstate_ );
        }

        info2 ti;
        ti.ident      = token_ident(token_type::END_OF_FILE);
        ti.where.line = lstate_.line;
        ti.where.pos  = std::distance( lstate_.line_itr, b );
        return ti;
    }

private:

    std::string       input_;
    lexer::token_trie trie_;
    info2             current_;
    info2             peek_;
    slice::iterator   iter_;
    position_state    lstate_;
};

int main_lex( )
{
    auto tr = mico::lexer::make_trie( );
    std::string test = "1e10!hello, \n"
                       "; 0xFFFF_FFFF_FFFF_FFFF\n"
                       "\"hello!\n"
                       " \n"
                       " \n"
                       "\"\n"
                       "<>!\n";

    auto bb = test.begin( );
    lexer2::position_state lstate(bb);

    lexer2 lex;
    lex.reset( test );

    while( !lex.eof( ) ) {
        std::cout << lex.current( ).where << "\t"
                  << lex.current( ).ident.name << "\t"
                  << lex.current( ).ident.lit( ) << " "
                  <<  "\n"
                      ;
        lex.advance( );
    }
    std::cout << lex.current( ).where << "\t"
              << lex.current( ).ident.name << "\t"
              << lex.current( ).ident.lit( ) << " "
              <<  "\n"
                  ;

    return 0;
}
