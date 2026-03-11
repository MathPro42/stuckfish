#include <gtest/gtest.h>

#include "../src/chess/bitboard.hh"

TEST(BitboardTests, CheckBitCorrectly)
{
    // Set all board to 0
    Bitboard board = 0ULL;
    EXPECT_FALSE(check_bit(board, Cases::A1))
        << "Should have return False but returned True.\n";
    board = ~0ULL;
    EXPECT_TRUE(check_bit(board, Cases::A1))
        << "Should have return False but returned True.\n";
}

TEST(BitboardTests, SetBitCorrectly)
{
    // Set all board to 0
    Bitboard board = 0ULL;

    // Set A1 (Bit 0)
    set_bit(board, Cases::A1);
    EXPECT_EQ(board, 1ULL) << "A1 should represent 1ULL.\nBoard looks like:\n"
                           << bitboard_to_string(board);

    // Set H8 (Bit 63)
    set_bit(board, Cases::H8);
    // 1ULL << 63 is the H8 bit. The + 1ULL is because A1 is still set!
    EXPECT_EQ(board, (1ULL << 63) | 1ULL)
        << "A1 and H8 should represent 1ULL.\nBoard looks like:\n"
        << bitboard_to_string(board);
}

TEST(BitboardTests, ClearBitCorrectly)
{
    // Set all board to 1
    Bitboard board = ~0ULL;

    clear_bit(board, Cases::A1);
    EXPECT_FALSE(check_bit(board, Cases::A1));
}

TEST(BitboardTests, ToggleBitCorrectly)
{
    // Set all board to 1
    Bitboard board = ~0ULL;

    toggle_bit(board, Cases::A1);
    EXPECT_FALSE(check_bit(board, Cases::A1))
        << "Failed to toggle from 1 to 0\n";
    toggle_bit(board, Cases::A1);
    EXPECT_TRUE(check_bit(board, Cases::A1))
        << "Failed to toggle from 0 to 1\n";
    ;
}
