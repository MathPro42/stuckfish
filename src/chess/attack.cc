#include "attack.hh"

#include "bitboard.hh"

Bitboard knight_attacks[64];
Bitboard king_attacks[64];
Bitboard pawn_attacks[2][64]; // [Color][Square]

void init_leapers()
{
    for (int i = 0; i < 64; i++)
    {
        Cases cases = static_cast<Cases>(i);

        knight_attacks[i] = mask_knight_attacks(cases);
        king_attacks[i] = mask_king_attacks(cases);
        pawn_attacks[BLACK][i] = mask_pawn_attacks(BLACK, cases);
        pawn_attacks[WHITE][i] = mask_pawn_attacks(WHITE, cases);
    }
}
