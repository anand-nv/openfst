// See www.openfst.org for extensive documentation on this weighted
// finite-state transducer library.

#include <fst/extensions/ngram/nthbit.h>

#include <fst/types.h>

namespace fst {

#if !defined(__BMI2__)  // BMI2 has everything in the header
#if SIZE_MAX == UINT32_MAX

// 32-bit platforms will be slow when using 64-bit operations; use this
// table-based version instead.  This only contains constant shifts, which
// have been benchmarked to be fast.

// These tables were generated using:
//
//  uint32 nth_bit_scan(uint64 v, uint32 r) {
//    for (int i = 0; i < 64; ++i) {
//      if ((r -= v & 1) == 0) return i;
//      v >>= 1;
//    }
//    return -1;
//  }
//
//  printf("static const uint8 nth_bit_bit_count[256] = {\n");
//  for (size_t i = 0; i < 256; ++i) {
//    printf("%d, ", __builtin_popcount(i));
//    if (i % 16 == 15) printf("\n");
//  }
//  printf("};\n");
//
//  printf("static const uint8 nth_bit_bit_pos[8][256] = {{\n");
//  for (size_t j = 0; j < 8; ++j) {
//    for (size_t i = 0; i < 256; ++i) {
//      uint8 pos = nth_bit_scan(i, j);
//      printf("%d, ", pos);
//      if (i % 16 == 15) printf("\n");
//    }
//    if (j != 7) printf("}, {\n");
//  }
//  printf("}};\n");
//
// This table contains the popcount of 1-byte values:
// nth_bit_bit_count[v] == __builtin_popcount(v).
static const uint8 nth_bit_bit_count[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4,
    2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
    4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};

// This table contains the bit position of the r-th set bit in v, for 1-byte v,
// (or 255 if there are fewer than r bits set, but those values are never used):
// nth_bit_bit_pos[r][v] == nth_bit_scan(v, r).
static const uint8 nth_bit_bit_pos[8][256] = {
    {
        255, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0,
        1,   0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
        2,   0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0,
        1,   0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0,
        3,   0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0,
        1,   0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0,
        2,   0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0,
        1,   0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4,   0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0,
        1,   0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
        2,   0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0,
        1,   0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    },
    {
        255, 255, 255, 1, 255, 2, 2, 1, 255, 3, 3,   1, 3, 2, 2,   1, 255, 4,
        4,   1,   4,   2, 2,   1, 4, 3, 3,   1, 3,   2, 2, 1, 255, 5, 5,   1,
        5,   2,   2,   1, 5,   3, 3, 1, 3,   2, 2,   1, 5, 4, 4,   1, 4,   2,
        2,   1,   4,   3, 3,   1, 3, 2, 2,   1, 255, 6, 6, 1, 6,   2, 2,   1,
        6,   3,   3,   1, 3,   2, 2, 1, 6,   4, 4,   1, 4, 2, 2,   1, 4,   3,
        3,   1,   3,   2, 2,   1, 6, 5, 5,   1, 5,   2, 2, 1, 5,   3, 3,   1,
        3,   2,   2,   1, 5,   4, 4, 1, 4,   2, 2,   1, 4, 3, 3,   1, 3,   2,
        2,   1,   255, 7, 7,   1, 7, 2, 2,   1, 7,   3, 3, 1, 3,   2, 2,   1,
        7,   4,   4,   1, 4,   2, 2, 1, 4,   3, 3,   1, 3, 2, 2,   1, 7,   5,
        5,   1,   5,   2, 2,   1, 5, 3, 3,   1, 3,   2, 2, 1, 5,   4, 4,   1,
        4,   2,   2,   1, 4,   3, 3, 1, 3,   2, 2,   1, 7, 6, 6,   1, 6,   2,
        2,   1,   6,   3, 3,   1, 3, 2, 2,   1, 6,   4, 4, 1, 4,   2, 2,   1,
        4,   3,   3,   1, 3,   2, 2, 1, 6,   5, 5,   1, 5, 2, 2,   1, 5,   3,
        3,   1,   3,   2, 2,   1, 5, 4, 4,   1, 4,   2, 2, 1, 4,   3, 3,   1,
        3,   2,   2,   1,
    },
    {
        255, 255, 255, 255, 255, 255, 255, 2, 255, 255, 255, 3, 255, 3, 3, 2,
        255, 255, 255, 4,   255, 4,   4,   2, 255, 4,   4,   3, 4,   3, 3, 2,
        255, 255, 255, 5,   255, 5,   5,   2, 255, 5,   5,   3, 5,   3, 3, 2,
        255, 5,   5,   4,   5,   4,   4,   2, 5,   4,   4,   3, 4,   3, 3, 2,
        255, 255, 255, 6,   255, 6,   6,   2, 255, 6,   6,   3, 6,   3, 3, 2,
        255, 6,   6,   4,   6,   4,   4,   2, 6,   4,   4,   3, 4,   3, 3, 2,
        255, 6,   6,   5,   6,   5,   5,   2, 6,   5,   5,   3, 5,   3, 3, 2,
        6,   5,   5,   4,   5,   4,   4,   2, 5,   4,   4,   3, 4,   3, 3, 2,
        255, 255, 255, 7,   255, 7,   7,   2, 255, 7,   7,   3, 7,   3, 3, 2,
        255, 7,   7,   4,   7,   4,   4,   2, 7,   4,   4,   3, 4,   3, 3, 2,
        255, 7,   7,   5,   7,   5,   5,   2, 7,   5,   5,   3, 5,   3, 3, 2,
        7,   5,   5,   4,   5,   4,   4,   2, 5,   4,   4,   3, 4,   3, 3, 2,
        255, 7,   7,   6,   7,   6,   6,   2, 7,   6,   6,   3, 6,   3, 3, 2,
        7,   6,   6,   4,   6,   4,   4,   2, 6,   4,   4,   3, 4,   3, 3, 2,
        7,   6,   6,   5,   6,   5,   5,   2, 6,   5,   5,   3, 5,   3, 3, 2,
        6,   5,   5,   4,   5,   4,   4,   2, 5,   4,   4,   3, 4,   3, 3, 2,
    },
    {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 3,   255, 255, 255, 255, 255, 255, 255, 4,   255, 255, 255, 4,
        255, 4,   4,   3,   255, 255, 255, 255, 255, 255, 255, 5,   255, 255,
        255, 5,   255, 5,   5,   3,   255, 255, 255, 5,   255, 5,   5,   4,
        255, 5,   5,   4,   5,   4,   4,   3,   255, 255, 255, 255, 255, 255,
        255, 6,   255, 255, 255, 6,   255, 6,   6,   3,   255, 255, 255, 6,
        255, 6,   6,   4,   255, 6,   6,   4,   6,   4,   4,   3,   255, 255,
        255, 6,   255, 6,   6,   5,   255, 6,   6,   5,   6,   5,   5,   3,
        255, 6,   6,   5,   6,   5,   5,   4,   6,   5,   5,   4,   5,   4,
        4,   3,   255, 255, 255, 255, 255, 255, 255, 7,   255, 255, 255, 7,
        255, 7,   7,   3,   255, 255, 255, 7,   255, 7,   7,   4,   255, 7,
        7,   4,   7,   4,   4,   3,   255, 255, 255, 7,   255, 7,   7,   5,
        255, 7,   7,   5,   7,   5,   5,   3,   255, 7,   7,   5,   7,   5,
        5,   4,   7,   5,   5,   4,   5,   4,   4,   3,   255, 255, 255, 7,
        255, 7,   7,   6,   255, 7,   7,   6,   7,   6,   6,   3,   255, 7,
        7,   6,   7,   6,   6,   4,   7,   6,   6,   4,   6,   4,   4,   3,
        255, 7,   7,   6,   7,   6,   6,   5,   7,   6,   6,   5,   6,   5,
        5,   3,   7,   6,   6,   5,   6,   5,   5,   4,   6,   5,   5,   4,
        5,   4,   4,   3,
    },
    {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 4,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 5,   255, 255, 255, 255, 255, 255, 255, 5,
        255, 255, 255, 5,   255, 5,   5,   4,   255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 6,   255, 255, 255, 255,
        255, 255, 255, 6,   255, 255, 255, 6,   255, 6,   6,   4,   255, 255,
        255, 255, 255, 255, 255, 6,   255, 255, 255, 6,   255, 6,   6,   5,
        255, 255, 255, 6,   255, 6,   6,   5,   255, 6,   6,   5,   6,   5,
        5,   4,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 7,   255, 255, 255, 255, 255, 255, 255, 7,   255, 255,
        255, 7,   255, 7,   7,   4,   255, 255, 255, 255, 255, 255, 255, 7,
        255, 255, 255, 7,   255, 7,   7,   5,   255, 255, 255, 7,   255, 7,
        7,   5,   255, 7,   7,   5,   7,   5,   5,   4,   255, 255, 255, 255,
        255, 255, 255, 7,   255, 255, 255, 7,   255, 7,   7,   6,   255, 255,
        255, 7,   255, 7,   7,   6,   255, 7,   7,   6,   7,   6,   6,   4,
        255, 255, 255, 7,   255, 7,   7,   6,   255, 7,   7,   6,   7,   6,
        6,   5,   255, 7,   7,   6,   7,   6,   6,   5,   7,   6,   6,   5,
        6,   5,   5,   4,
    },
    {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 5,   255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 6,   255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 6,
        255, 255, 255, 255, 255, 255, 255, 6,   255, 255, 255, 6,   255, 6,
        6,   5,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 7,   255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 7,   255, 255, 255, 255, 255, 255,
        255, 7,   255, 255, 255, 7,   255, 7,   7,   5,   255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 7,   255, 255,
        255, 255, 255, 255, 255, 7,   255, 255, 255, 7,   255, 7,   7,   6,
        255, 255, 255, 255, 255, 255, 255, 7,   255, 255, 255, 7,   255, 7,
        7,   6,   255, 255, 255, 7,   255, 7,   7,   6,   255, 7,   7,   6,
        7,   6,   6,   5,
    },
    {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 6,   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 7,   255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 7,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 7,   255, 255, 255, 255, 255, 255, 255, 7,   255, 255, 255, 7,
        255, 7,   7,   6,
    },
    {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 7,
    }};

uint32 nth_bit(const uint64 v, uint32 r) {
  DCHECK_NE(v, 0);
  DCHECK_LE(0, r);
  DCHECK_LT(r, __builtin_popcountll(v));

  uint32 next_byte = v & 255;
  uint32 byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 8) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 8 + nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 16) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 16 + nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 24) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 24 + nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 32) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 32 + nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 40) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 40 + nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 48) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 48 + nth_bit_bit_pos[r][next_byte];
  r -= byte_popcount;
  next_byte = (v >> 56) & 255;
  byte_popcount = nth_bit_bit_count[next_byte];
  if (r < byte_popcount) return 56 + nth_bit_bit_pos[r][next_byte];
  return -1;
}

#elif SIZE_MAX == UINT64_MAX  // 64-bit, non-BMI2
// These tables are generated using:
//
//  constexpr uint64 kOnesStep8 = 0x0101010101010101;
//  printf("const uint64 kPrefixSumOverflow[64] = {\n");
//  for (int k = 0; k < 64; ++k) {
//    printf("  0x%" FST_LL_FORMAT "x,\n",  (0x7F - k) * kOnesStep8);
//  }
//  printf("};\n");
//
//  printf("const uint8 kSelectInByte[8 * 256] = {\n");
//  for (int j = 0; j < 8; ++j) {
//    for (int i = 0; i < 256; ++i) {
//      if (i > 0) printf(" ");
//      if (i % 16 == 0) printf("\n  ");
//      const int k = findbitn(i, j);
//      printf("%d,", k == -1 ? 0 : k);
//    }
//    printf("\n");
//  }
//  printf("};\n");
//
namespace internal {

// clang-format off
const uint64 kPrefixSumOverflow[64] = {
  0x7f7f7f7f7f7f7f7f,
  0x7e7e7e7e7e7e7e7e,
  0x7d7d7d7d7d7d7d7d,
  0x7c7c7c7c7c7c7c7c,
  0x7b7b7b7b7b7b7b7b,
  0x7a7a7a7a7a7a7a7a,
  0x7979797979797979,
  0x7878787878787878,
  0x7777777777777777,
  0x7676767676767676,
  0x7575757575757575,
  0x7474747474747474,
  0x7373737373737373,
  0x7272727272727272,
  0x7171717171717171,
  0x7070707070707070,
  0x6f6f6f6f6f6f6f6f,
  0x6e6e6e6e6e6e6e6e,
  0x6d6d6d6d6d6d6d6d,
  0x6c6c6c6c6c6c6c6c,
  0x6b6b6b6b6b6b6b6b,
  0x6a6a6a6a6a6a6a6a,
  0x6969696969696969,
  0x6868686868686868,
  0x6767676767676767,
  0x6666666666666666,
  0x6565656565656565,
  0x6464646464646464,
  0x6363636363636363,
  0x6262626262626262,
  0x6161616161616161,
  0x6060606060606060,
  0x5f5f5f5f5f5f5f5f,
  0x5e5e5e5e5e5e5e5e,
  0x5d5d5d5d5d5d5d5d,
  0x5c5c5c5c5c5c5c5c,
  0x5b5b5b5b5b5b5b5b,
  0x5a5a5a5a5a5a5a5a,
  0x5959595959595959,
  0x5858585858585858,
  0x5757575757575757,
  0x5656565656565656,
  0x5555555555555555,
  0x5454545454545454,
  0x5353535353535353,
  0x5252525252525252,
  0x5151515151515151,
  0x5050505050505050,
  0x4f4f4f4f4f4f4f4f,
  0x4e4e4e4e4e4e4e4e,
  0x4d4d4d4d4d4d4d4d,
  0x4c4c4c4c4c4c4c4c,
  0x4b4b4b4b4b4b4b4b,
  0x4a4a4a4a4a4a4a4a,
  0x4949494949494949,
  0x4848484848484848,
  0x4747474747474747,
  0x4646464646464646,
  0x4545454545454545,
  0x4444444444444444,
  0x4343434343434343,
  0x4242424242424242,
  0x4141414141414141,
  0x4040404040404040
};

const uint8 kSelectInByte[8 * 256] = {
  0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,

  0, 0, 0, 1, 0, 2, 2, 1, 0, 3, 3, 1, 3, 2, 2, 1,
  0, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  0, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1,
  5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  0, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1,
  6, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1,
  5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  0, 7, 7, 1, 7, 2, 2, 1, 7, 3, 3, 1, 3, 2, 2, 1,
  7, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  7, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1,
  5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  7, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1,
  6, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
  6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1,
  5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,

  0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 3, 3, 2,
  0, 0, 0, 4, 0, 4, 4, 2, 0, 4, 4, 3, 4, 3, 3, 2,
  0, 0, 0, 5, 0, 5, 5, 2, 0, 5, 5, 3, 5, 3, 3, 2,
  0, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,
  0, 0, 0, 6, 0, 6, 6, 2, 0, 6, 6, 3, 6, 3, 3, 2,
  0, 6, 6, 4, 6, 4, 4, 2, 6, 4, 4, 3, 4, 3, 3, 2,
  0, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2,
  6, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,
  0, 0, 0, 7, 0, 7, 7, 2, 0, 7, 7, 3, 7, 3, 3, 2,
  0, 7, 7, 4, 7, 4, 4, 2, 7, 4, 4, 3, 4, 3, 3, 2,
  0, 7, 7, 5, 7, 5, 5, 2, 7, 5, 5, 3, 5, 3, 3, 2,
  7, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,
  0, 7, 7, 6, 7, 6, 6, 2, 7, 6, 6, 3, 6, 3, 3, 2,
  7, 6, 6, 4, 6, 4, 4, 2, 6, 4, 4, 3, 4, 3, 3, 2,
  7, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2,
  6, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
  0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 4, 0, 4, 4, 3,
  0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 5, 5, 3,
  0, 0, 0, 5, 0, 5, 5, 4, 0, 5, 5, 4, 5, 4, 4, 3,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 3,
  0, 0, 0, 6, 0, 6, 6, 4, 0, 6, 6, 4, 6, 4, 4, 3,
  0, 0, 0, 6, 0, 6, 6, 5, 0, 6, 6, 5, 6, 5, 5, 3,
  0, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 3,
  0, 0, 0, 7, 0, 7, 7, 4, 0, 7, 7, 4, 7, 4, 4, 3,
  0, 0, 0, 7, 0, 7, 7, 5, 0, 7, 7, 5, 7, 5, 5, 3,
  0, 7, 7, 5, 7, 5, 5, 4, 7, 5, 5, 4, 5, 4, 4, 3,
  0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 3,
  0, 7, 7, 6, 7, 6, 6, 4, 7, 6, 6, 4, 6, 4, 4, 3,
  0, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 3,
  7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
  0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 5, 5, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 4,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 5,
  0, 0, 0, 6, 0, 6, 6, 5, 0, 6, 6, 5, 6, 5, 5, 4,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 4,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 5,
  0, 0, 0, 7, 0, 7, 7, 5, 0, 7, 7, 5, 7, 5, 5, 4,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6,
  0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 4,
  0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 5,
  0, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 6, 6, 5,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 5,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6,
  0, 0, 0, 7, 0, 7, 7, 6, 0, 7, 7, 6, 7, 6, 6, 5,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
  0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 7, 7, 6,

  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7
};
// clang-format on

}  // namespace internal
#endif                        // 64-bit, non-BMI2
#endif                        // !defined(__BMI2__)

}  // namespace fst
