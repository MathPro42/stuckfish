#include "attack.hh"

#include "bitboard.hh"
#include "magic.hh"

Bitboard knight_attacks[64];
Bitboard king_attacks[64];
Bitboard pawn_attacks[2][64]; // [Color][Square]

Bitboard rook_magic_numbers[64];
Bitboard bishop_magic_numbers[64];
Bitboard rook_attacks[64][4096];
Bitboard bishop_attacks[64][512];

const int rook_bits[64] = { 12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10,
                            10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10,
                            10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10,
                            11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
                            10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12 };

const int bishop_bits[64] = { 6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5,
                              5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
                              5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5,
                              5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6 };

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

void init_sliders()
{
    for (int square = 0; square < 64; square++)
    {
        Cases cases = static_cast<Cases>(square);

        bishop_magic_numbers[square] =
            find_magic_number(cases, bishop_bits[square], true);

        Bitboard b_mask = mask_bishop_pre_attacks(cases);
        int b_indices = 1 << bishop_bits[square];

        for (int i = 0; i < b_indices; i++)
        {
            Bitboard occupancy = set_occupancy(i, bishop_bits[square], b_mask);
            int magic_index = (occupancy * bishop_magic_numbers[square])
                >> (64 - bishop_bits[square]);
            bishop_attacks[square][magic_index] =
                bishop_attacks_on_the_fly(cases, occupancy);
        }

        rook_magic_numbers[square] =
            find_magic_number(cases, rook_bits[square], false);

        Bitboard r_mask = mask_rook_pre_attacks(cases);
        int r_indices = 1 << rook_bits[square];

        for (int i = 0; i < r_indices; i++)
        {
            Bitboard occupancy = set_occupancy(i, rook_bits[square], r_mask);
            int magic_index = (occupancy * rook_magic_numbers[square])
                >> (64 - rook_bits[square]);
            rook_attacks[square][magic_index] =
                rook_attacks_on_the_fly(cases, occupancy);
        }
    }
}

Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask)
{
    Bitboard occupancy = 0ULL;

    for (int count = 0; count < bits_in_mask; count++)
    {
        int cases = std::countr_zero(attack_mask);

        attack_mask &= attack_mask - 1;

        if (index & (1 << count))
        {
            occupancy |= (1ULL << cases);
        }
    }

    return occupancy;
}

Bitboard rook_attacks_on_the_fly(Cases cases, Bitboard block)
{
    Bitboard attack = 0ULL;
    int digit = cases / 8;
    int letter = cases % 8;

    for (int i = digit + 1; i <= 7; i++)
    {
        set_bit(attack, static_cast<Cases>(i * 8 + letter));
        if ((1ULL << (i * 8 + letter)) & block)
            break;
    }
    for (int i = digit - 1; i >= 0; i--)
    {
        set_bit(attack, static_cast<Cases>(i * 8 + letter));
        if ((1ULL << (i * 8 + letter)) & block)
            break;
    }
    for (int j = letter + 1; j <= 7; j++)
    {
        set_bit(attack, static_cast<Cases>(digit * 8 + j));
        if ((1ULL << (digit * 8 + j)) & block)
            break;
    }
    for (int j = letter - 1; j >= 0; j--)
    {
        set_bit(attack, static_cast<Cases>(digit * 8 + j));
        if ((1ULL << (digit * 8 + j)) & block)
            break;
    }

    return attack;
}

Bitboard bishop_attacks_on_the_fly(Cases cases, Bitboard block)
{
    Bitboard attack = 0ULL;
    int digit = cases / 8;
    int letter = cases % 8;

    for (int i = digit + 1, j = letter + 1; i <= 7 && j <= 7; i++, j++)
    {
        set_bit(attack, static_cast<Cases>(i * 8 + j));
        if ((1ULL << (i * 8 + j)) & block)
            break;
    }
    for (int i = digit + 1, j = letter - 1; i <= 7 && j >= 0; i++, j--)
    {
        set_bit(attack, static_cast<Cases>(i * 8 + j));
        if ((1ULL << (i * 8 + j)) & block)
            break;
    }
    for (int i = digit - 1, j = letter + 1; i >= 0 && j <= 7; i--, j++)
    {
        set_bit(attack, static_cast<Cases>(i * 8 + j));
        if ((1ULL << (i * 8 + j)) & block)
            break;
    }
    for (int i = digit - 1, j = letter - 1; i >= 0 && j >= 0; i--, j--)
    {
        set_bit(attack, static_cast<Cases>(i * 8 + j));
        if ((1ULL << (i * 8 + j)) & block)
            break;
    }

    return attack;
}

Bitboard generate_attacks(const BoardState& state, Color attacking_color)
{
    Bitboard attacks = 0ULL;

    Bitboard blockers = state.colors[WHITE] | state.colors[BLACK];

    Bitboard pawns = state.pieces[PAWN] & state.colors[attacking_color];
    while (pawns)
    {
        int cases = std::countr_zero(pawns);
        attacks |= pawn_attacks[attacking_color][cases];
        pawns &= pawns - 1;
    }

    Bitboard knights = state.pieces[KNIGHT] & state.colors[attacking_color];
    while (knights)
    {
        int cases = std::countr_zero(knights);
        attacks |= knight_attacks[cases];
        knights &= knights - 1;
    }

    Bitboard king = state.pieces[KING] & state.colors[attacking_color];
    while (king)
    {
        int cases = std::countr_zero(king);
        king |= king_attacks[cases];
        king &= king - 1;
    }

    Bitboard diagonals = (state.pieces[BISHOP] | state.pieces[QUEEN])
        & state.colors[attacking_color];
    while (diagonals)
    {
        int cases = std::countr_zero(diagonals);

        Bitboard mask = mask_bishop_pre_attacks(static_cast<Cases>(cases));
        Bitboard occupancy = blockers & mask;
        int magic_index = static_cast<int>(
            (static_cast<uint64_t>(occupancy)
             * static_cast<uint64_t>(bishop_magic_numbers[cases]))
            >> (64 - bishop_bits[cases]));
        attacks |= bishop_attacks[cases][magic_index];

        diagonals &= diagonals - 1;
    }
    Bitboard orthogonal = (state.pieces[ROOK] | state.pieces[QUEEN])
        & state.colors[attacking_color];
    while (orthogonal)
    {
        int cases = std::countr_zero(orthogonal);

        Bitboard mask = mask_rook_pre_attacks(static_cast<Cases>(cases));
        Bitboard occupancy = blockers & mask;
        int magic_index = static_cast<int>(
            (static_cast<uint64_t>(occupancy)
             * static_cast<uint64_t>(rook_magic_numbers[cases]))
            >> (64 - rook_bits[cases]));
        attacks |= rook_attacks[cases][magic_index];

        orthogonal &= orthogonal - 1;
    }
    return attacks;
}
