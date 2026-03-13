#pragma once

#include "attack.hh"

constexpr Bitboard mask_knight_attacks(Cases cases)
{
    Bitboard knight = 1ULL << cases;
    Bitboard attack = 0ULL;

    attack |= (knight << 17) & NOT_A;
    attack |= (knight << 10) & NOT_AB;
    attack |= (knight >> 15) & NOT_A;
    attack |= (knight >> 6) & NOT_AB;

    attack |= (knight << 15) & NOT_H;
    attack |= (knight << 6) & NOT_GH;
    attack |= (knight >> 17) & NOT_H;
    attack |= (knight >> 10) & NOT_GH;

    return attack;
}

constexpr Bitboard mask_king_attacks(Cases cases)
{
    Bitboard king = 1ULL << cases;
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

constexpr Bitboard mask_pawn_attacks(Color color, Cases cases)
{
    Bitboard pawn = 1ULL << cases;
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
constexpr Bitboard mask_rook_pre_attacks(Cases cases)
{
    Bitboard attack = 0ULL;
    int digit = cases / 8;
    int letter = cases % 8;

    for (int i = digit + 1; i <= 6; i++)
        set_bit(attack, static_cast<Cases>(i * 8 + letter));
    for (int i = digit - 1; i >= 1; i--)
        set_bit(attack, static_cast<Cases>(i * 8 + letter));
    for (int j = letter + 1; j <= 6; j++)
        set_bit(attack, static_cast<Cases>(digit * 8 + j));
    for (int j = letter - 1; j >= 1; j--)
        set_bit(attack, static_cast<Cases>(digit * 8 + j));

    return attack;
}

constexpr Bitboard mask_bishop_pre_attacks(Cases cases)
{
    Bitboard attack = 0ULL;
    int digit = cases / 8;
    int letter = cases % 8;

    for (int i = digit + 1, j = letter + 1; i <= 6 && j <= 6; i++, j++)
        set_bit(attack, static_cast<Cases>(i * 8 + j));
    for (int i = digit + 1, j = letter - 1; i <= 6 && j >= 1; i++, j--)
        set_bit(attack, static_cast<Cases>(i * 8 + j));
    for (int i = digit - 1, j = letter + 1; i >= 1 && j <= 6; i--, j++)
        set_bit(attack, static_cast<Cases>(i * 8 + j));
    for (int i = digit - 1, j = letter - 1; i >= 1 && j >= 1; i--, j--)
        set_bit(attack, static_cast<Cases>(i * 8 + j));

    return attack;
}
