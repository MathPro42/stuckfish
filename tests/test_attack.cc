#include <gtest/gtest.h>

#include "../src/chess/attack.hh"
#include "../src/chess/bitboard.hh"

TEST(AttackTests, KnightD4)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, C6);
    set_bit(expected, E6);
    set_bit(expected, B5);
    set_bit(expected, F5);
    set_bit(expected, B3);
    set_bit(expected, F3);
    set_bit(expected, C2);
    set_bit(expected, E2);

    EXPECT_EQ(knight_attacks[D4], expected)
        << "Knight on D4 failed.\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(knight_attacks[D4]);
}

TEST(AttackTests, KnightA1)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, B3);
    set_bit(expected, C2);

    EXPECT_EQ(knight_attacks[A1], expected)
        << "Knight on A1 failed. Edge wrapping bug detected!\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(knight_attacks[A1]);
}

TEST(AttackTests, KingD4)
{
    init_leapers();
    Bitboard expected = 0ULL;

    set_bit(expected, C4);
    set_bit(expected, E4);

    set_bit(expected, C5);
    set_bit(expected, D5);
    set_bit(expected, E5);

    set_bit(expected, C3);
    set_bit(expected, D3);
    set_bit(expected, E3);

    EXPECT_EQ(king_attacks[D4], expected)
        << "King on D4 failed.\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(king_attacks[D4]);
}

TEST(AttackTests, KingA1)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, B1);
    set_bit(expected, B2);
    set_bit(expected, A2);

    EXPECT_EQ(king_attacks[A1], expected)
        << "King on A1 failed. Edge wrapping bug detected!\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(king_attacks[A1]);
}

TEST(AttackTests, WhitePawnE4)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, D5);
    set_bit(expected, F5);

    EXPECT_EQ(pawn_attacks[WHITE][E4], expected)
        << "White Pawn on E4 failed.\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(pawn_attacks[WHITE][E4]);
}

TEST(AttackTests, BlackPawnD5)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, C4);
    set_bit(expected, E4);

    EXPECT_EQ(pawn_attacks[BLACK][D5], expected)
        << "Black Pawn on D5 failed.\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(pawn_attacks[BLACK][D5]);
}

TEST(AttackTests, WhitePawnA4)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, B5);

    EXPECT_EQ(pawn_attacks[WHITE][A4], expected)
        << "White Pawn on A4 failed.\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(pawn_attacks[WHITE][A4]);
}

TEST(AttackTests, BlackPawnH5)
{
    init_leapers();

    Bitboard expected = 0ULL;
    set_bit(expected, G4);

    EXPECT_EQ(pawn_attacks[BLACK][H5], expected)
        << "Black Pawn on H5 failed.\nExpected:\n"
        << bitboard_to_string(expected) << "\nActual:\n"
        << bitboard_to_string(pawn_attacks[BLACK][H5]);
}

TEST(AttackTests, RookMagicD4WithBlockers)
{
    init_sliders();

    Cases cases = D4;

    Bitboard blockers = 0ULL;
    set_bit(blockers, D7);
    set_bit(blockers, B4);

    Bitboard masked_blockers = blockers & mask_rook_pre_attacks(cases);

    int magic_index = (masked_blockers * rook_magic_numbers[cases])
        >> (64 - rook_bits[cases]);

    Bitboard actual_attacks = rook_attacks[cases][magic_index];

    Bitboard expected_attacks = 0ULL;

    set_bit(expected_attacks, D5);
    set_bit(expected_attacks, D6);
    set_bit(expected_attacks, D7);
    set_bit(expected_attacks, D3);
    set_bit(expected_attacks, D2);
    set_bit(expected_attacks, D1);
    set_bit(expected_attacks, E4);
    set_bit(expected_attacks, F4);
    set_bit(expected_attacks, G4);
    set_bit(expected_attacks, H4);
    set_bit(expected_attacks, C4);
    set_bit(expected_attacks, B4);

    EXPECT_EQ(actual_attacks, expected_attacks)
        << "Magic Rook on D4 failed to respect blockers.\nExpected:\n"
        << bitboard_to_string(expected_attacks) << "\nActual:\n"
        << bitboard_to_string(actual_attacks);
}

TEST(AttackTests, GenerateAttackMap)
{
    init_leapers();
    init_sliders();

    BoardState state;
    // FEN: White Rook on A1, White Pawn on B2. Black King on C3.
    load_fen(state, "8/8/8/8/8/2k5/1P6/R7 b - - 0 1");

    Bitboard white_attacks = generate_attacks(state, WHITE);

    EXPECT_TRUE(check_bit(white_attacks, Cases::A3))
        << "Pawn attack on A3 missing";
    EXPECT_TRUE(check_bit(white_attacks, Cases::C3))
        << "Pawn attack on C3 missing";

    EXPECT_TRUE(check_bit(white_attacks, Cases::A5))
        << "Rook attack on A5 missing";
    EXPECT_TRUE(check_bit(white_attacks, Cases::H1))
        << "Rook attack on H1 missing";
    EXPECT_FALSE(check_bit(white_attacks, Cases::B2))
        << "Rook should not attack B2";
}
