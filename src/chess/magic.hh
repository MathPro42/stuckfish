#pragma once

#include "bitboard.hh"

inline Bitboard get_random_U64();
inline Bitboard get_magic_candidate();
Bitboard find_magic_number(Cases cases, int relevant_bits, bool is_bishop);
