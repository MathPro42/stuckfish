#include "magic.hh"

#include <random>

#include "attack.hh"
#include "bitboard.hh"

inline Bitboard get_random_U64()
{
    static std::mt19937_64 rng(1804289383);
    std::uniform_int_distribution<Bitboard> dist;
    return dist(rng);
}

inline Bitboard get_magic_candidate()
{
    return get_random_U64() & get_random_U64() & get_random_U64();
}

Bitboard find_magic_number(Cases cases, int relevant_bits, bool is_bishop)
{
    Bitboard occupancies[4096];
    Bitboard attacks[4096];

    int occupancy_indices = 1 << relevant_bits;

    Bitboard attack_mask = is_bishop ? mask_bishop_pre_attacks(cases)
                                     : mask_rook_pre_attacks(cases);

    for (int i = 0; i < occupancy_indices; i++)
    {
        occupancies[i] = set_occupancy(i, relevant_bits, attack_mask);
        attacks[i] = is_bishop
            ? bishop_attacks_on_the_fly(cases, occupancies[i])
            : rook_attacks_on_the_fly(cases, occupancies[i]);
    }

    while (true)
    {
        Bitboard magic = get_magic_candidate();

        if (std::popcount((attack_mask * magic) & 0xFF00000000000000ULL) < 6)
            continue;

        Bitboard used_attack[4096] = { 0ULL };
        bool failed = false;

        for (int i = 0; i < occupancy_indices; i++)
        {
            int magic_index = (occupancies[i] * magic) >> (64 - relevant_bits);

            if (used_attack[magic_index] == 0ULL)
            {
                used_attack[magic_index] = attacks[i];
            }
            else if (used_attack[magic_index] != attacks[i])
            {
                failed = true;
                break;
            }
        }
        if (!failed)
        {
            return magic;
        }
    }
}
