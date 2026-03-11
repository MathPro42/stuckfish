#include "bitboard.hh"

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
