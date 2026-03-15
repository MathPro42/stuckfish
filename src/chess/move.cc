#include "move.hh"

#include "attack.hh"
#include "bitboard.hh"

constexpr uint8_t castling_rights_mask[64] = {
    13, 15, 15, 15, 12, 15, 15, 14, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 7,  15, 15, 15, 3,  15, 15, 11
};

// Helper to identify capture victim during generation
static inline int get_cap(const BoardState& state, int to)
{
    for (int i = PAWN; i <= KING; i++)
    {
        if (check_bit(state.pieces[i], static_cast<Cases>(to)))
            return i;
    }
    return 6; // None
}

template <Color color>
void generate_pawn_moves(const BoardState& state, MoveList& move_list)
{
    Bitboard empty = ~(state.colors[WHITE] | state.colors[BLACK]);
    Bitboard pawns = state.pieces[PAWN] & state.colors[color];
    Bitboard enemies = state.colors[color ^ 1];

    constexpr int fwd = (color == WHITE) ? 8 : -8;
    auto shift = [](Bitboard b, int s) {
        return (s > 0) ? (b << s) : (b >> -s);
    };

    Bitboard pushes = shift(pawns, fwd) & empty;
    for (Cases to : Bitloop(pushes))
    {
        int from = to - fwd;
        if ((color == WHITE && to >= A8) || (color == BLACK && to <= H1))
        {
            move_list.add(Move(from, to, PAWN, 6, PR_QUEEN));
            move_list.add(Move(from, to, PAWN, 6, PR_ROOK));
            move_list.add(Move(from, to, PAWN, 6, PR_BISHOP));
            move_list.add(Move(from, to, PAWN, 6, PR_KNIGHT));
        }
        else
        {
            move_list.add(Move(from, to, PAWN, 6, QUIET_MOVE));
            if ((color == WHITE && from >= A2 && from <= H2)
                || (color == BLACK && from >= A7 && from <= H7))
            {
                if (check_bit(empty, static_cast<Cases>(to + fwd)))
                    move_list.add(
                        Move(from, to + fwd, PAWN, 6, DOUBLE_PAWN_PUSH));
            }
        }
    }

    auto gen_pawn_caps = [&](int offset, Bitboard mask) {
        Bitboard caps = shift(pawns, offset) & mask & enemies;
        for (Cases to : Bitloop(caps))
        {
            int from = to - offset;
            int victim = get_cap(state, to);
            if ((color == WHITE && to >= A8) || (color == BLACK && to <= H1))
            {
                move_list.add(Move(from, to, PAWN, victim, PC_QUEEN));
                move_list.add(Move(from, to, PAWN, victim, PC_ROOK));
                move_list.add(Move(from, to, PAWN, victim, PC_BISHOP));
                move_list.add(Move(from, to, PAWN, victim, PC_KNIGHT));
            }
            else
                move_list.add(Move(from, to, PAWN, victim, CAPTURE));
        }
    };
    gen_pawn_caps((color == WHITE ? 7 : -9), NOT_H);
    gen_pawn_caps((color == WHITE ? 9 : -7), NOT_A);

    if (state.en_passant_target != NO_CASES)
    {
        Bitboard ep_bit = 1ULL << state.en_passant_target;
        Bitboard l_caps =
            shift(pawns, (color == WHITE ? 7 : -9)) & NOT_H & ep_bit;
        for (Cases to : Bitloop(l_caps))
            move_list.add(Move(to - (color == WHITE ? 7 : -9), to, PAWN, PAWN,
                               EP_CAPTURE));
        Bitboard r_caps =
            shift(pawns, (color == WHITE ? 9 : -7)) & NOT_A & ep_bit;
        for (Cases to : Bitloop(r_caps))
            move_list.add(Move(to - (color == WHITE ? 9 : -7), to, PAWN, PAWN,
                               EP_CAPTURE));
    }
}

void generate_piece_moves(const BoardState& state, Color color,
                          MoveList& move_list)
{
    Bitboard valid = ~state.colors[color];
    Bitboard enemies = state.colors[color ^ 1];
    Bitboard blockers = state.colors[WHITE] | state.colors[BLACK];

    auto gen_leapers = [&](Bitboard pieces, int pt, Bitboard* attack_table) {
        for (Cases from : Bitloop(pieces))
        {
            Bitboard moves = attack_table[from] & valid;
            for (Cases to : Bitloop(moves))
            {
                int victim = check_bit(enemies, to) ? get_cap(state, to) : 6;
                move_list.add(Move(from, to, pt, victim,
                                   (victim == 6 ? QUIET_MOVE : CAPTURE)));
            }
        }
    };
    gen_leapers(state.pieces[KNIGHT] & state.colors[color], KNIGHT,
                knight_attacks);
    gen_leapers(state.pieces[KING] & state.colors[color], KING, king_attacks);

    Bitboard diag =
        (state.pieces[BISHOP] | state.pieces[QUEEN]) & state.colors[color];
    for (Cases from : Bitloop(diag))
    {
        Bitboard occ = blockers & bishop_masks[from];
        int idx =
            (occ * bishop_magic_numbers[from]) >> (64 - bishop_bits[from]);
        Bitboard moves = bishop_attacks[from][idx] & valid;
        int pt = check_bit(state.pieces[QUEEN], from) ? QUEEN : BISHOP;
        for (Cases to : Bitloop(moves))
        {
            int victim = check_bit(enemies, to) ? get_cap(state, to) : 6;
            move_list.add(Move(from, to, pt, victim,
                               (victim == 6 ? QUIET_MOVE : CAPTURE)));
        }
    }

    Bitboard orth =
        (state.pieces[ROOK] | state.pieces[QUEEN]) & state.colors[color];
    for (Cases from : Bitloop(orth))
    {
        Bitboard occ = blockers & rook_masks[from];
        int idx = (occ * rook_magic_numbers[from]) >> (64 - rook_bits[from]);
        Bitboard moves = rook_attacks[from][idx] & valid;
        int pt = check_bit(state.pieces[QUEEN], from) ? QUEEN : ROOK;
        for (Cases to : Bitloop(moves))
        {
            int victim = check_bit(enemies, to) ? get_cap(state, to) : 6;
            move_list.add(Move(from, to, pt, victim,
                               (victim == 6 ? QUIET_MOVE : CAPTURE)));
        }
    }
}

void generate_all_moves(const BoardState& state, MoveList& move_list)
{
    if (state.player == WHITE)
        generate_pawn_moves<WHITE>(state, move_list);
    else
        generate_pawn_moves<BLACK>(state, move_list);

    generate_piece_moves(state, state.player, move_list);

    Bitboard blockers = state.colors[WHITE] | state.colors[BLACK];
    if (state.player == WHITE)
    {
        if ((state.castling_rights & 1) && !check_bit(blockers, F1)
            && !check_bit(blockers, G1))
            if (!is_cases_attacked(state, E1, BLACK)
                && !is_cases_attacked(state, F1, BLACK)
                && !is_cases_attacked(state, G1, BLACK))
                move_list.add(Move(E1, G1, KING, 6, KING_CASTLE));
        if ((state.castling_rights & 2) && !check_bit(blockers, B1)
            && !check_bit(blockers, C1) && !check_bit(blockers, D1))
            if (!is_cases_attacked(state, E1, BLACK)
                && !is_cases_attacked(state, D1, BLACK)
                && !is_cases_attacked(state, C1, BLACK))
                move_list.add(Move(E1, C1, KING, 6, QUEEN_CASTLE));
    }
    else
    {
        if ((state.castling_rights & 4) && !check_bit(blockers, F8)
            && !check_bit(blockers, G8))
            if (!is_cases_attacked(state, E8, WHITE)
                && !is_cases_attacked(state, F8, WHITE)
                && !is_cases_attacked(state, G8, WHITE))
                move_list.add(Move(E8, G8, KING, 6, KING_CASTLE));
        if ((state.castling_rights & 8) && !check_bit(blockers, B8)
            && !check_bit(blockers, C8) && !check_bit(blockers, D8))
            if (!is_cases_attacked(state, E8, WHITE)
                && !is_cases_attacked(state, D8, WHITE)
                && !is_cases_attacked(state, C8, WHITE))
                move_list.add(Move(E8, C8, KING, 6, QUEEN_CASTLE));
    }
}

bool make_move(BoardState& state, Move move)
{
    int from = move.get_from(), to = move.get_to();
    int piece = move.get_piece(), cap = move.get_captured(),
        flags = move.get_flags();
    Color us = state.player, them = static_cast<Color>(us ^ 1);

    if (flags == EP_CAPTURE)
    {
        int cap_sq = (us == WHITE) ? to - 8 : to + 8;
        clear_bit(state.pieces[PAWN], static_cast<Cases>(cap_sq));
        clear_bit(state.colors[them], static_cast<Cases>(cap_sq));
    }
    else if (cap != 6)
    {
        clear_bit(state.pieces[cap], static_cast<Cases>(to));
        clear_bit(state.colors[them], static_cast<Cases>(to));
    }

    clear_bit(state.pieces[piece], static_cast<Cases>(from));
    clear_bit(state.colors[us], static_cast<Cases>(from));

    if (flags >= PR_KNIGHT)
        set_bit(state.pieces[(flags & 3) + 1], static_cast<Cases>(to));
    else
        set_bit(state.pieces[piece], static_cast<Cases>(to));
    set_bit(state.colors[us], static_cast<Cases>(to));

    if (flags == KING_CASTLE)
    {
        Cases r_from = (us == WHITE) ? H1 : H8, r_to = (us == WHITE) ? F1 : F8;
        clear_bit(state.pieces[ROOK], r_from);
        clear_bit(state.colors[us], r_from);
        set_bit(state.pieces[ROOK], r_to);
        set_bit(state.colors[us], r_to);
    }
    else if (flags == QUEEN_CASTLE)
    {
        Cases r_from = (us == WHITE) ? A1 : A8, r_to = (us == WHITE) ? D1 : D8;
        clear_bit(state.pieces[ROOK], r_from);
        clear_bit(state.colors[us], r_from);
        set_bit(state.pieces[ROOK], r_to);
        set_bit(state.colors[us], r_to);
    }

    state.en_passant_target = (flags == DOUBLE_PAWN_PUSH)
        ? static_cast<Cases>((us == WHITE) ? to - 8 : to + 8)
        : NO_CASES;
    state.castling_rights &= castling_rights_mask[from];
    state.castling_rights &= castling_rights_mask[to];
    state.player = them;

    int king_sq = std::countr_zero(state.pieces[KING] & state.colors[us]);
    return !is_cases_attacked(state, static_cast<Cases>(king_sq), them);
}

void unmake_move(BoardState& state, Move move, uint8_t old_rights, Cases old_ep)
{
    state.player = static_cast<Color>(state.player ^ 1);
    int from = move.get_from(), to = move.get_to();
    int piece = move.get_piece(), cap = move.get_captured(),
        flags = move.get_flags();
    Color us = state.player, them = static_cast<Color>(us ^ 1);

    clear_bit(state.colors[us], static_cast<Cases>(to));
    if (flags >= PR_KNIGHT)
        clear_bit(state.pieces[(flags & 3) + 1], static_cast<Cases>(to));
    else
        clear_bit(state.pieces[piece], static_cast<Cases>(to));

    set_bit(state.pieces[piece], static_cast<Cases>(from));
    set_bit(state.colors[us], static_cast<Cases>(from));

    if (flags == EP_CAPTURE)
    {
        int cap_sq = (us == WHITE) ? to - 8 : to + 8;
        set_bit(state.pieces[PAWN], static_cast<Cases>(cap_sq));
        set_bit(state.colors[them], static_cast<Cases>(cap_sq));
    }
    else if (cap != 6)
    {
        set_bit(state.pieces[cap], static_cast<Cases>(to));
        set_bit(state.colors[them], static_cast<Cases>(to));
    }

    if (flags == KING_CASTLE)
    {
        Cases r_from = (us == WHITE) ? H1 : H8, r_to = (us == WHITE) ? F1 : F8;
        clear_bit(state.pieces[ROOK], r_to);
        clear_bit(state.colors[us], r_to);
        set_bit(state.pieces[ROOK], r_from);
        set_bit(state.colors[us], r_from);
    }
    else if (flags == QUEEN_CASTLE)
    {
        Cases r_from = (us == WHITE) ? A1 : A8, r_to = (us == WHITE) ? D1 : D8;
        clear_bit(state.pieces[ROOK], r_to);
        clear_bit(state.colors[us], r_to);
        set_bit(state.pieces[ROOK], r_from);
        set_bit(state.colors[us], r_from);
    }

    state.castling_rights = old_rights;
    state.en_passant_target = old_ep;
}

uint64_t perft(BoardState& state, int depth)
{
    MoveList move_list;
    generate_all_moves(state, move_list);

    uint64_t nodes = 0;
    for (int i = 0; i < move_list.count; i++)
    {
        uint8_t r = state.castling_rights;
        Cases ep = state.en_passant_target;
        if (make_move(state, move_list.moves[i]))
        {
            if (depth > 1)
                nodes += perft(state, depth - 1);
            else
                nodes += 1;
            unmake_move(state, move_list.moves[i], r, ep);
        }
        else
        {
            unmake_move(state, move_list.moves[i], r, ep);
        }
    }
    return nodes;
}
