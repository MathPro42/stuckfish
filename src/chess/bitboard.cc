#include "bitboard.hh"

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

#include <sstream>
#include <string>

void load_fen(BoardState& boardstate, std::string_view fen)
{
    for (Bitboard& board : boardstate.colors)
        board = 0ULL;
    for (Bitboard& board : boardstate.pieces)
        board = 0ULL;

    std::istringstream iss{ std::string(fen) };
    std::string token;

    iss >> token;
    int digit = 7;
    int letter = 0;

    for (char c : token)
    {
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
        Color color = std::isupper(c) ? WHITE : BLACK;

        set_bit(boardstate.colors[color], square);

        switch (std::tolower(c))
        {
        case 'p':
            set_bit(boardstate.pieces[PAWN], square);
            break;
        case 'n':
            set_bit(boardstate.pieces[KNIGHT], square);
            break;
        case 'b':
            set_bit(boardstate.pieces[BISHOP], square);
            break;
        case 'r':
            set_bit(boardstate.pieces[ROOK], square);
            break;
        case 'q':
            set_bit(boardstate.pieces[QUEEN], square);
            break;
        case 'k':
            set_bit(boardstate.pieces[KING], square);
            break;
        default:
            throw std::invalid_argument("Invalid FEN character");
        }
        letter++;
    }

    iss >> token;
    boardstate.player = (token == "w") ? WHITE : BLACK;

    iss >> token;
    boardstate.castling_rights = 0;
    if (token != "-")
    {
        for (char c : token)
        {
            if (c == 'K')
                boardstate.castling_rights |= 1;
            if (c == 'Q')
                boardstate.castling_rights |= 2;
            if (c == 'k')
                boardstate.castling_rights |= 4;
            if (c == 'q')
                boardstate.castling_rights |= 8;
        }
    }

    iss >> token;
    if (token != "-")
    {
        int letter = token[0] - 'a';
        int digit = token[1] - '1';
        boardstate.en_passant_target = static_cast<Cases>(digit * 8 + letter);
    }
    else
    {
        boardstate.en_passant_target = NO_CASES;
    }

    iss >> boardstate.half_move_clock >> boardstate.full_move_number;
}
