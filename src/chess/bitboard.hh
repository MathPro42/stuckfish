#pragma once

#include <cstdint>
#include <string>

using Bitboard = uint64_t;

enum Color
{
    WHITE = 0,
    BLACK = 1
};

enum Pieces
{
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};

enum Cases
{
    A1 = 0,
    B1 = 1,
    C1 = 2,
    D1 = 3,
    E1 = 4,
    F1 = 5,
    G1 = 6,
    H1 = 7,
    A2 = 8,
    B2 = 9,
    C2 = 10,
    D2 = 11,
    E2 = 12,
    F2 = 13,
    G2 = 14,
    H2 = 15,
    A3 = 16,
    B3 = 17,
    C3 = 18,
    D3 = 19,
    E3 = 20,
    F3 = 21,
    G3 = 22,
    H3 = 23,
    A4 = 24,
    B4 = 25,
    C4 = 26,
    D4 = 27,
    E4 = 28,
    F4 = 29,
    G4 = 30,
    H4 = 31,
    A5 = 32,
    B5 = 33,
    C5 = 34,
    D5 = 35,
    E5 = 36,
    F5 = 37,
    G5 = 38,
    H5 = 39,
    A6 = 40,
    B6 = 41,
    C6 = 42,
    D6 = 43,
    E6 = 44,
    F6 = 45,
    G6 = 46,
    H6 = 47,
    A7 = 48,
    B7 = 49,
    C7 = 50,
    D7 = 51,
    E7 = 52,
    F7 = 53,
    G7 = 54,
    H7 = 55,
    A8 = 56,
    B8 = 57,
    C8 = 58,
    D8 = 59,
    E8 = 60,
    F8 = 61,
    G8 = 62,
    H8 = 63,
    NO = 64,
};

struct BoardState
{
    Bitboard pieces[6]; // represent all pieces of each type
    Bitboard colors[2]; // represent all pieces for each player

    Color player = WHITE;
    // Bit 0: White Kingside (K)
    // Bit 1: White Queenside (Q)
    // Bit 2: Black Kingside (k)
    // Bit 3: Black Queenside (q)
    uint8_t castling_rights = 0;
    Cases en_passant_target = NO;
    int half_move_clock = 0;
    int full_move_number = 1;
};

// Set the case bit to 1
inline void set_bit(Bitboard& board, Cases cases);

// Set the case bit to 0
inline void clear_bit(Bitboard& board, Cases cases);

// Check if there is a piece on this case
inline bool check_bit(Bitboard board, Cases cases);

// Switch the value on the cases (if case is 1 then case become 0 and vice
// versa)
inline void toggle_bit(Bitboard& board, Cases cases);

// convert a bitboard in a string of 8x8 looking a chess board
std::pmr::string bitboard_to_string(Bitboard board);
// convert the board in a string of 8x8 looking a chess board
std::pmr::string board_to_string(BoardState board);

// convert fen representation in bitboard
void load_fen(BoardState& boardstate, std::string_view fen);

#include "bitboard.hxx"
