#include <gtest/gtest.h>

#include "../src/chess/attack.hh"
#include "../src/chess/move.hh"

TEST(MoveGenerationTests, PerftStartingPosition)
{
    init_leapers();
    init_sliders();

    BoardState state;
    load_fen(state, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    EXPECT_EQ(perft(state, 1), 20ULL) << "Depth 1 failed\n";
    EXPECT_EQ(perft(state, 2), 400ULL) << "Depth 2 failed\n";

    EXPECT_EQ(perft(state, 3), 8902ULL) << "Depth 3 failed\n";

    EXPECT_EQ(perft(state, 4), 197281ULL) << "Depth 4 failed\n";
    EXPECT_EQ(perft(state, 5), 4865609ULL) << "Depth 5 failed\n";
}

TEST(MoveGenerationTests, PerftKiwipetePosition)
{
    init_leapers();
    init_sliders();

    BoardState state;
    load_fen(
        state,
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    EXPECT_EQ(perft(state, 1), 48ULL) << "Depth 1 failed\n";
    EXPECT_EQ(perft(state, 2), 2039ULL) << "Depth 2 failed\n";

    EXPECT_EQ(perft(state, 3), 97862ULL) << "Depth 3 failed\n";

    EXPECT_EQ(perft(state, 4), 4085603ULL) << "Depth 4 failed\n";
    EXPECT_EQ(perft(state, 5), 193690690ULL) << "Depth 5 failed\n";
}

TEST(MoveGenerationTests, PerftPerformancePosition)
{
    init_leapers();
    init_sliders();

    BoardState state;
    load_fen(state, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    EXPECT_EQ(perft(state, 7), 3195901860) << "Depth 7 failed\n";
}
