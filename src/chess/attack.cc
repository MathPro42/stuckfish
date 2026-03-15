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

Bitboard rook_masks[64];
Bitboard bishop_masks[64];

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
        bishop_masks[square] = mask_bishop_pre_attacks(cases);

        int b_indices = 1 << bishop_bits[square];
        for (int i = 0; i < b_indices; i++)
        {
            Bitboard occupancy =
                set_occupancy(i, bishop_bits[square], bishop_masks[square]);
            int magic_index = (occupancy * bishop_magic_numbers[square])
                >> (64 - bishop_bits[square]);
            bishop_attacks[square][magic_index] =
                bishop_attacks_on_the_fly(cases, occupancy);
        }

        rook_magic_numbers[square] =
            find_magic_number(cases, rook_bits[square], false);
        rook_masks[square] = mask_rook_pre_attacks(cases);

        int r_indices = 1 << rook_bits[square];
        for (int i = 0; i < r_indices; i++)
        {
            Bitboard occupancy =
                set_occupancy(i, rook_bits[square], rook_masks[square]);
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
    for (Cases cases : Bitloop(pawns))
    {
        attacks |= pawn_attacks[attacking_color][cases];
    }

    Bitboard knights = state.pieces[KNIGHT] & state.colors[attacking_color];
    for (Cases cases : Bitloop(knights))
    {
        attacks |= knight_attacks[cases];
    }

    Bitboard king = state.pieces[KING] & state.colors[attacking_color];
    for (Cases cases : Bitloop(king))
    {
        attacks |= king_attacks[cases];
    }

    Bitboard diagonals = (state.pieces[BISHOP] | state.pieces[QUEEN])
        & state.colors[attacking_color];
    for (Cases cases : Bitloop(diagonals))
    {
        Bitboard mask = mask_bishop_pre_attacks(cases);
        Bitboard occupancy = blockers & mask;
        int magic_index = static_cast<int>(
            (static_cast<uint64_t>(occupancy)
             * static_cast<uint64_t>(bishop_magic_numbers[cases]))
            >> (64 - bishop_bits[cases]));
        attacks |= bishop_attacks[cases][magic_index];
    }

    Bitboard orthogonal = (state.pieces[ROOK] | state.pieces[QUEEN])
        & state.colors[attacking_color];
    for (Cases cases : Bitloop(orthogonal))
    {
        Bitboard mask = mask_rook_pre_attacks(cases);
        Bitboard occupancy = blockers & mask;
        int magic_index = static_cast<int>(
            (static_cast<uint64_t>(occupancy)
             * static_cast<uint64_t>(rook_magic_numbers[cases]))
            >> (64 - rook_bits[cases]));
        attacks |= rook_attacks[cases][magic_index];
    }

    return attacks;
}

bool is_cases_attacked(const BoardState& state, Cases cases,
                       Color attacking_color)
{
    if (knight_attacks[cases] & state.pieces[KNIGHT]
        & state.colors[attacking_color])
        return true;
    if (king_attacks[cases] & state.pieces[KING]
        & state.colors[attacking_color])
        return true;

    Color defending_color = (attacking_color == WHITE) ? BLACK : WHITE;
    if (pawn_attacks[defending_color][cases] & state.pieces[PAWN]
        & state.colors[attacking_color])
        return true;

    Bitboard blockers = state.colors[WHITE] | state.colors[BLACK];

    Bitboard b_occupancy = blockers & bishop_masks[cases];
    int b_magic = static_cast<int>((b_occupancy * bishop_magic_numbers[cases])
                                   >> (64 - bishop_bits[cases]));
    if (bishop_attacks[cases][b_magic]
        & (state.pieces[BISHOP] | state.pieces[QUEEN])
        & state.colors[attacking_color])
        return true;

    Bitboard r_occupancy = blockers & rook_masks[cases];
    int r_magic = static_cast<int>((r_occupancy * rook_magic_numbers[cases])
                                   >> (64 - rook_bits[cases]));
    if (rook_attacks[cases][r_magic]
        & (state.pieces[ROOK] | state.pieces[QUEEN])
        & state.colors[attacking_color])
        return true;

    return false;
}
