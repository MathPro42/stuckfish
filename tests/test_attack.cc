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
