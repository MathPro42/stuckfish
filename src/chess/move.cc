#include "move.hh"

#include "attack.hh"
#include "bitboard.hh"

constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;
constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;

constexpr uint8_t castling_rights_mask[64] = {
    13, 15, 15, 15, 12, 15, 15, 14, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 7,  15, 15, 15, 3,  15, 15, 11
};

template <Color color>
void generate_pawn_moves(const BoardState& state, MoveList& move_list)
{
    Bitboard empty_cases = ~(state.colors[WHITE] | state.colors[BLACK]);
    Bitboard pawns = state.pieces[PAWN] & state.colors[color];
    Bitboard enemies = state.colors[color == WHITE ? BLACK : WHITE];

    constexpr int fwd = (color == WHITE) ? 8 : -8;
    constexpr int cap_l = (color == WHITE) ? 7 : -7;
    constexpr int cap_r = (color == WHITE) ? 9 : -9;

    constexpr Bitboard rank_promo = (color == WHITE) ? RANK_3 : RANK_6;
    constexpr Bitboard mask_l = (color == WHITE) ? NOT_H : NOT_A;
    constexpr Bitboard mask_r = (color == WHITE) ? NOT_A : NOT_H;

    auto shift = [](Bitboard b, int s) {
        if (s > 0)
            return b << s;
        else
            return b >> -s;
    };

    auto add_moves = [&](Bitboard bb, int offset, int flag,
                         bool check_promo = false, bool is_cap = false) {
        while (bb)
        {
            int to = std::countr_zero(bb);
            int from = to - offset;

            if (check_promo
                && ((color == WHITE) ? (to >= Cases::A8) : (to <= Cases::H1)))
            {
                move_list.add(Move(from, to, is_cap ? PC_QUEEN : PR_QUEEN));
                move_list.add(Move(from, to, is_cap ? PC_ROOK : PR_ROOK));
                move_list.add(Move(from, to, is_cap ? PC_BISHOP : PR_BISHOP));
                move_list.add(Move(from, to, is_cap ? PC_KNIGHT : PR_KNIGHT));
            }
            else
            {
                move_list.add(Move(from, to, flag));
            }
            bb &= bb - 1;
        }
    };

    Bitboard single_pushes = shift(pawns, fwd) & empty_cases;
    add_moves(single_pushes, fwd, QUIET_MOVE, true, false);
    add_moves(shift(single_pushes & rank_promo, fwd) & empty_cases, fwd * 2,
              DOUBLE_PAWN_PUSH);

    add_moves(shift(pawns, cap_l) & mask_l & enemies, cap_l, CAPTURE, true,
              true);
    add_moves(shift(pawns, cap_r) & mask_r & enemies, cap_r, CAPTURE, true,
              true);

    if (state.en_passant_target != NO_CASES)
    {
        Bitboard ep_cases = 1ULL << state.en_passant_target;
        add_moves(shift(pawns, cap_l) & mask_l & ep_cases, cap_l, EP_CAPTURE);
        add_moves(shift(pawns, cap_r) & mask_r & ep_cases, cap_r, EP_CAPTURE);
    }
}

void generate_piece_moves(const BoardState& state, Color color,
                          MoveList& move_list)
{
    Bitboard valid_cases = ~state.colors[color];
    Bitboard blockers = state.colors[WHITE] | state.colors[BLACK];
    Bitboard enemy_pieces = state.colors[color == WHITE ? BLACK : WHITE];

    Bitboard knights = state.pieces[KNIGHT] & state.colors[color];
    while (knights)
    {
        int from_cases = std::countr_zero(knights);

        Bitboard moves = knight_attacks[from_cases] & valid_cases;

        while (moves)
        {
            int to_cases = std::countr_zero(moves);

            MoveFlags flag =
                check_bit(enemy_pieces, static_cast<Cases>(to_cases))
                ? CAPTURE
                : QUIET_MOVE;

            move_list.add(Move(from_cases, to_cases, flag));
            moves &= moves - 1;
        }
        knights &= knights - 1;
    }

    Bitboard king = state.pieces[KING] & state.colors[color];
    while (king)
    {
        int from_cases = std::countr_zero(king);

        Bitboard moves = king_attacks[from_cases] & valid_cases;

        while (moves)
        {
            int to_cases = std::countr_zero(moves);

            MoveFlags flag =
                check_bit(enemy_pieces, static_cast<Cases>(to_cases))
                ? CAPTURE
                : QUIET_MOVE;

            move_list.add(Move(from_cases, to_cases, flag));
            moves &= moves - 1;
        }
        king &= king - 1;
    }

    Bitboard diagonals =
        (state.pieces[BISHOP] | state.pieces[QUEEN]) & state.colors[color];
    while (diagonals)
    {
        int from_cases = std::countr_zero(diagonals);

        Bitboard mask = mask_bishop_pre_attacks(static_cast<Cases>(from_cases));
        Bitboard occupancy = blockers & mask;
        int magic_index = static_cast<int>(
            (static_cast<uint64_t>(occupancy)
             * static_cast<uint64_t>(bishop_magic_numbers[from_cases]))
            >> (64 - bishop_bits[from_cases]));

        Bitboard moves = bishop_attacks[from_cases][magic_index] & valid_cases;

        while (moves)
        {
            int to_cases = std::countr_zero(moves);
            MoveFlags flag =
                check_bit(enemy_pieces, static_cast<Cases>(to_cases))
                ? CAPTURE
                : QUIET_MOVE;
            move_list.add(Move(from_cases, to_cases, flag));
            moves &= moves - 1;
        }
        diagonals &= diagonals - 1;
    }

    Bitboard orthogonals =
        (state.pieces[ROOK] | state.pieces[QUEEN]) & state.colors[color];
    while (orthogonals)
    {
        int from_cases = std::countr_zero(orthogonals);

        Bitboard mask = mask_rook_pre_attacks(static_cast<Cases>(from_cases));
        Bitboard occupancy = blockers & mask;
        int magic_index = static_cast<int>(
            (static_cast<uint64_t>(occupancy)
             * static_cast<uint64_t>(rook_magic_numbers[from_cases]))
            >> (64 - rook_bits[from_cases]));

        Bitboard moves = rook_attacks[from_cases][magic_index] & valid_cases;

        while (moves)
        {
            int to_cases = std::countr_zero(moves);
            MoveFlags flag =
                check_bit(enemy_pieces, static_cast<Cases>(to_cases))
                ? CAPTURE
                : QUIET_MOVE;
            move_list.add(Move(from_cases, to_cases, flag));
            moves &= moves - 1;
        }
        orthogonals &= orthogonals - 1;
    }
}

void generate_all_moves(const BoardState& state, MoveList& move_list)
{
    Color us = state.player;
    Color them = (us == WHITE) ? BLACK : WHITE;

    if (us == WHITE)
    {
        generate_pawn_moves<WHITE>(state, move_list);
    }
    else
    {
        generate_pawn_moves<BLACK>(state, move_list);
    }
    generate_piece_moves(state, us, move_list);

    Bitboard blockers = state.colors[WHITE] | state.colors[BLACK];

    Bitboard enemy_attacks = generate_attacks(state, them);

    if (us == WHITE)
    {
        if (state.castling_rights & 1)
        {
            if (!check_bit(blockers, F1) && !check_bit(blockers, G1))
            {
                if (!check_bit(enemy_attacks, E1)
                    && !check_bit(enemy_attacks, F1)
                    && !check_bit(enemy_attacks, G1))
                {
                    move_list.add(Move(E1, G1, KING_CASTLE));
                }
            }
        }
        if (state.castling_rights & 2)
        {
            if (!check_bit(blockers, B1) && !check_bit(blockers, C1)
                && !check_bit(blockers, D1))
            {
                if (!check_bit(enemy_attacks, E1)
                    && !check_bit(enemy_attacks, D1)
                    && !check_bit(enemy_attacks, C1))
                {
                    move_list.add(Move(E1, C1, QUEEN_CASTLE));
                }
            }
        }
    }
    else
    {
        if (state.castling_rights & 4)
        {
            if (!check_bit(blockers, F8) && !check_bit(blockers, G8))
            {
                if (!check_bit(enemy_attacks, E8)
                    && !check_bit(enemy_attacks, F8)
                    && !check_bit(enemy_attacks, G8))
                {
                    move_list.add(Move(E8, G8, KING_CASTLE));
                }
            }
        }
        if (state.castling_rights & 8)
        {
            if (!check_bit(blockers, B8) && !check_bit(blockers, C8)
                && !check_bit(blockers, D8))
            {
                if (!check_bit(enemy_attacks, E8)
                    && !check_bit(enemy_attacks, D8)
                    && !check_bit(enemy_attacks, C8))
                {
                    move_list.add(Move(E8, C8, QUEEN_CASTLE));
                }
            }
        }
    }
}

bool make_move(BoardState& state, Move move)
{
    int from = move.get_from();
    int to = move.get_to();
    int flags = move.get_flags();

    Color us = state.player;
    Color them = (us == WHITE) ? BLACK : WHITE;

    int moving_piece = PAWN;
    for (int i = 0; i < 6; i++)
    {
        if (check_bit(state.pieces[i], static_cast<Cases>(from)))
        {
            moving_piece = i;
            break;
        }
    }

    if (flags == CAPTURE || flags == EP_CAPTURE || flags >= PC_KNIGHT)
    {
        int capture_cases = to;

        if (flags == EP_CAPTURE)
        {
            capture_cases = (us == WHITE) ? to - 8 : to + 8;
        }

        for (int i = 0; i < 6; i++)
        {
            clear_bit(state.pieces[i], static_cast<Cases>(capture_cases));
        }
        clear_bit(state.colors[them], static_cast<Cases>(capture_cases));
    }

    clear_bit(state.pieces[moving_piece], static_cast<Cases>(from));
    set_bit(state.pieces[moving_piece], static_cast<Cases>(to));
    clear_bit(state.colors[us], static_cast<Cases>(from));
    set_bit(state.colors[us], static_cast<Cases>(to));

    if (flags >= PR_KNIGHT)
    {
        for (int i = 0; i < 6; i++)
        {
            clear_bit(state.pieces[i], static_cast<Cases>(to));
        }
        switch (flags)
        {
        case PR_KNIGHT:
        case PC_KNIGHT:
            set_bit(state.pieces[KNIGHT], static_cast<Cases>(to));
            break;

        case PR_BISHOP:
        case PC_BISHOP:
            set_bit(state.pieces[BISHOP], static_cast<Cases>(to));
            break;

        case PR_ROOK:
        case PC_ROOK:
            set_bit(state.pieces[ROOK], static_cast<Cases>(to));
            break;

        case PR_QUEEN:
        case PC_QUEEN:
            set_bit(state.pieces[QUEEN], static_cast<Cases>(to));
            break;
        }
    }

    if (flags == KING_CASTLE)
    {
        if (us == WHITE)
        {
            clear_bit(state.pieces[ROOK], H1);
            clear_bit(state.colors[WHITE], H1);

            set_bit(state.pieces[ROOK], F1);
            set_bit(state.colors[WHITE], F1);
        }
        else
        {
            clear_bit(state.pieces[ROOK], H8);
            clear_bit(state.colors[BLACK], H8);

            set_bit(state.pieces[ROOK], F8);
            set_bit(state.colors[BLACK], F8);
        }
    }
    if (flags == QUEEN_CASTLE)
    {
        if (us == WHITE)
        {
            clear_bit(state.pieces[ROOK], A1);
            clear_bit(state.colors[WHITE], A1);

            set_bit(state.pieces[ROOK], D1);
            set_bit(state.colors[WHITE], D1);
        }
        else
        {
            clear_bit(state.pieces[ROOK], A8);
            clear_bit(state.colors[BLACK], A8);

            set_bit(state.pieces[ROOK], D8);
            set_bit(state.colors[BLACK], D8);
        }
    }

    state.en_passant_target = NO_CASES;
    if (flags == DOUBLE_PAWN_PUSH)
    {
        state.en_passant_target =
            static_cast<Cases>((us == WHITE) ? to - 8 : to + 8);
    }
    state.castling_rights &= castling_rights_mask[from];
    state.castling_rights &= castling_rights_mask[to];
    state.player = them;

    Bitboard our_king = state.pieces[KING] & state.colors[us];
    int king_cases = std::countr_zero(our_king);

    Bitboard enemy_attacks = generate_attacks(state, them);

    if (check_bit(enemy_attacks, static_cast<Cases>(king_cases)))
    {
        return false;
    }

    return true;
}

uint64_t perft(BoardState state, int depth)
{
    if (depth == 0)
    {
        return 1ULL;
    }

    uint64_t nodes = 0;
    MoveList move_list;
    generate_all_moves(state, move_list);

    for (int i = 0; i < move_list.count; i++)
    {
        BoardState next_state = state;
        if (make_move(next_state, move_list.moves[i]))
        {
            nodes += perft(next_state, depth - 1);
        }
    }

    return nodes;
}
