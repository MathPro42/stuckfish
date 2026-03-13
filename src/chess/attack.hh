#pragma once
#include "bitboard.hh"

extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];
extern Bitboard pawn_attacks[2][64]; // [Color][Cases]

extern Bitboard rook_magic_numbers[64];
extern Bitboard bishop_magic_numbers[64];

extern Bitboard rook_attacks[64][4096];
extern Bitboard bishop_attacks[64][512];

extern const int rook_bits[64];
extern const int bishop_bits[64];

constexpr Bitboard NOT_A = 0xfefefefefefefefeULL;
constexpr Bitboard NOT_H = 0x7f7f7f7f7f7f7f7fULL;
constexpr Bitboard NOT_AB = 0xfcfcfcfcfcfcfcfcULL;
constexpr Bitboard NOT_GH = 0x3f3f3f3f3f3f3f3fULL;

constexpr Bitboard mask_knight_attacks(Cases cases);
constexpr Bitboard mask_king_attacks(Cases cases);
constexpr Bitboard mask_pawn_attacks(Color color, Cases cases);

constexpr Bitboard mask_rook_pre_attacks(Cases cases);
constexpr Bitboard mask_bishop_pre_attacks(Cases cases);

void init_leapers();
void init_sliders();
Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask);

Bitboard rook_attacks_on_the_fly(Cases cases, Bitboard block);
Bitboard bishop_attacks_on_the_fly(Cases cases, Bitboard block);

#include "attack.hxx"
