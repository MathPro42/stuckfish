#pragma once
#include <cstdint>

#include "bitboard.hh"

enum MoveFlags : uint16_t
{
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EP_CAPTURE = 5,
    // Promotions (8 to 15)
    PR_KNIGHT = 8,
    PR_BISHOP = 9,
    PR_ROOK = 10,
    PR_QUEEN = 11,
    // Promotion + Capture
    PC_KNIGHT = 12,
    PC_BISHOP = 13,
    PC_ROOK = 14,
    PC_QUEEN = 15
};

struct Move
{
    uint32_t data; // Bits: 0-5: to, 6-11: from, 12-14: piece, 15-17: captured,
                   // 18-21: flags
    Move()
        : data(0)
    {}

    Move(int from, int to, int piece, int captured, int flags)
    {
        data =
            (flags << 18) | (captured << 15) | (piece << 12) | (from << 6) | to;
    }

    constexpr uint16_t get_to() const
    {
        return data & 0x3F;
    }
    constexpr uint16_t get_from() const
    {
        return (data >> 6) & 0x3F;
    }
    constexpr uint16_t get_piece() const
    {
        return (data >> 12) & 0x07;
    }
    constexpr uint16_t get_captured() const
    {
        return (data >> 15) & 0x07;
    }
    constexpr uint16_t get_flags() const
    {
        return (data >> 18) & 0x0F;
    }
};

struct MoveList
{
    Move moves[256];
    int count = 0;

    void add(Move move)
    {
        moves[count] = move;
        count++;
    }
};

template <Color color>
void generate_pawn_moves(const BoardState& state, MoveList& move_list);

void generate_piece_moves(const BoardState& state, Color color,
                          MoveList& move_list);

[[nodiscard]] bool make_move(BoardState& state, Move move);
uint64_t perft(BoardState& state, int depth);
