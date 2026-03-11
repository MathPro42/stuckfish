#include <gtest/gtest.h>

#include "../src/chess/bitboard.hh"

TEST(BitboardTests, CheckBit)
{
    // Set all board to 0
    Bitboard board = 0ULL;
    EXPECT_FALSE(check_bit(board, A1))
        << "Should have return False but returned True.\n";
    board = ~0ULL;
    EXPECT_TRUE(check_bit(board, A1))
        << "Should have return False but returned True.\n";
}

TEST(BitboardTests, SetBit)
{
    // Set all board to 0
    Bitboard board = 0ULL;

    // Set A1 (Bit 0)
    set_bit(board, A1);
    EXPECT_EQ(board, 1ULL) << "A1 should represent 1ULL.\nBoard looks like:\n"
                           << bitboard_to_string(board);

    // Set H8 (Bit 63)
    set_bit(board, H8);
    // 1ULL << 63 is the H8 bit. The + 1ULL is because A1 is still set!
    EXPECT_EQ(board, (1ULL << 63) | 1ULL)
        << "A1 and H8 should represent 1ULL.\nBoard looks like:\n"
        << bitboard_to_string(board);
}

TEST(BitboardTests, ClearBit)
{
    // Set all board to 1
    Bitboard board = ~0ULL;

    clear_bit(board, A1);
    EXPECT_FALSE(check_bit(board, A1));
}

TEST(BitboardTests, ToggleBit)
{
    // Set all board to 1
    Bitboard board = ~0ULL;

    toggle_bit(board, A1);
    EXPECT_FALSE(check_bit(board, A1)) << "Failed to toggle from 1 to 0\n";
    toggle_bit(board, A1);
    EXPECT_TRUE(check_bit(board, A1)) << "Failed to toggle from 0 to 1\n";
    ;
}

TEST(BitboardTests, LoadFen)
{
    BoardState boardState;
    boardState.colors[WHITE] = ~0ULL;
    boardState.colors[BLACK] = ~0ULL;

    load_fen(boardState, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    EXPECT_TRUE(check_bit(boardState.colors[WHITE], E1))
        << "Expected White but none found";
    EXPECT_TRUE(check_bit(boardState.pieces[KING], E1))
        << "Expected King but none found";

    EXPECT_TRUE(check_bit(boardState.colors[BLACK], C8))
        << "Expected Black but none found";
    EXPECT_TRUE(check_bit(boardState.pieces[BISHOP], C8))
        << "Expected Bishop but none found";

    EXPECT_FALSE(check_bit(boardState.colors[WHITE], E4))
        << "Expected none but White found";
    EXPECT_FALSE(check_bit(boardState.colors[BLACK], E4))
        << "Expected none but Black found";
}
