#include "bitboard.hh"

#include <format>
#include <stdexcept>

std::pmr::string bitboard_to_string(Bitboard board)
{
    std::pmr::string string_board = "";
    for (int i = 7; i >= 0; i--)
    {
        for (int j = 0; j <= 7; j++)
        {
            if (check_bit(board, static_cast<Cases>(i * 8 + j)))
            {
                string_board += "1";
            }
            else
            {
                string_board += "0";
            }
        }
        string_board += "\n";
    }
    return string_board;
}

std::pmr::string board_to_string(BoardState board)
{
    return bitboard_to_string(board.colors[0] | board.colors[1]);
}

void load_fen(BoardState& boardstate, std::string_view fen)
{
    for (Bitboard& board : boardstate.colors)
    {
        board = 0ULL;
    }
    for (Bitboard& board : boardstate.pieces)
    {
        board = 0ULL;
    }

    int digit = 7; // Rank 8 (index 7)
    int letter = 0; // File A (index 0)

    for (char c : fen)
    {
        if (c == ' ')
            break;

        if (c == '/')
        {
            digit--;
            letter = 0;
            continue;
        }

        if (std::isdigit(c))
        {
            letter += (c - '0');
            continue;
        }

        Cases square = static_cast<Cases>(digit * 8 + letter);
        Color color = std::isupper(c) ? Color::WHITE : Color::BLACK;

        set_bit(boardstate.colors[color], square);

        switch (std::tolower(c))
        {
        case 'p':
            set_bit(boardstate.pieces[Pieces::PAWN], square);
            break;
        case 'n':
            set_bit(boardstate.pieces[Pieces::KNIGHT], square);
            break;
        case 'b':
            set_bit(boardstate.pieces[Pieces::BISHOP], square);
            break;
        case 'r':
            set_bit(boardstate.pieces[Pieces::ROOK], square);
            break;
        case 'q':
            set_bit(boardstate.pieces[Pieces::QUEEN], square);
            break;
        case 'k':
            set_bit(boardstate.pieces[Pieces::KING], square);
            break;
        default:
            throw std::invalid_argument("Invalid FEN character");
        }

        letter++;
    }
}
