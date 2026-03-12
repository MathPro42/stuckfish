#pragma once
#include "bitboard.hh"

extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];
extern Bitboard pawn_attacks[2][64]; // [Color][Square]

constexpr Bitboard NOT_A = 0xfefefefefefefefeULL;
constexpr Bitboard NOT_H = 0x7f7f7f7f7f7f7f7fULL;
constexpr Bitboard NOT_AB = 0xfcfcfcfcfcfcfcfcULL;
constexpr Bitboard NOT_GH = 0x3f3f3f3f3f3f3f3fULL;

constexpr Bitboard mask_knight_attacks(Cases cases);
constexpr Bitboard mask_king_attacks(Cases cases);

void init_leapers();

#include "attack.hxx"
constexpr Bitboard mask_pawn_attacks(Color color, Cases cases);
