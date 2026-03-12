#pragma once

#include "attack.hh"
#include "bitboard.hh"

constexpr Bitboard mask_knight_attacks(Cases square)
{
    Bitboard knight = 1ULL << square;
    Bitboard attack = 0ULL;

    attack |= (knight << 17) & NOT_A;
    attack |= (knight << 10) & NOT_AB;
    attack |= (knight >> 15) & NOT_A;
    attack |= (knight >> 6) & NOT_AB;

    // Moving WEST (Mask out H or GH)
    attack |= (knight << 15) & NOT_H;
    attack |= (knight << 6) & NOT_GH;
    attack |= (knight >> 17) & NOT_H;
    attack |= (knight >> 10) & NOT_GH;

    return attack;
}

constexpr Bitboard mask_king_attacks(Cases square)
{
    Bitboard king = 1ULL << square;
    Bitboard attack = 0ULL;

    attack |= (king << 8);
    attack |= (king >> 8);

    attack |= (king << 9) & NOT_A;
    attack |= (king >> 7) & NOT_A;
    attack |= (king << 1) & NOT_A;

    attack |= (king >> 9) & NOT_H;
    attack |= (king << 7) & NOT_H;
    attack |= (king >> 1) & NOT_H;

    return attack;
}

constexpr Bitboard mask_pawn_attacks(Color color, Cases square)
{
    Bitboard pawn = 1ULL << square;
    Bitboard attacks = 0ULL;

    if (color == WHITE)
    {
        attacks |= (pawn << 7) & NOT_H;
        attacks |= (pawn << 9) & NOT_A;
    }
    else
    {
        attacks |= (pawn & NOT_H) >> 7;
        attacks |= (pawn & NOT_A) >> 9;
    }
    return attacks;
}
