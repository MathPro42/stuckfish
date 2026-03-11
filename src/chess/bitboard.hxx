#pragma once

#include "bitboard.hh"

inline void set_bit(Bitboard& board, Cases cases)
{
    board |= (1ULL << cases);
}

inline void clear_bit(Bitboard& board, Cases cases)
{
    board &= ~(1ULL << cases);
}

inline bool check_bit(Bitboard board, Cases cases)
{
    return (board & (1ULL << cases)) != 0;
}

inline void toggle_bit(Bitboard& board, Cases cases)
{
    board ^= (1ULL << cases);
}
