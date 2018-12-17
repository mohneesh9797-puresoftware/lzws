// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "read_code.h"
#include "common.h"
#include "utils.h"

static inline void add_byte(lzws_code_fast_t* code_ptr, uint_fast8_t* code_bits_ptr, uint_fast8_t byte, bool msb) {
  lzws_code_fast_t code      = *code_ptr;
  uint_fast8_t     code_bits = *code_bits_ptr;
}

lzws_result_t lzws_decompressor_read_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  uint_fast8_t code_bits             = state_ptr->last_used_code_bits;
  uint_fast8_t source_remainder_bits = state_ptr->source_remainder_bits;

  // Source remainder bits will always be <= code bits - 1.
  // So source bytes will always be >= 1.
  uint_fast8_t source_bytes = lzws_ceil_bits_to_bytes(code_bits - source_remainder_bits);
  if (*source_length_ptr < source_bytes) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  lzws_code_fast_t current_code      = state_ptr->source_remainder;
  uint_fast8_t     current_code_bits = source_remainder_bits;
  bool             msb               = state_ptr->msb;

  do {
    uint_fast8_t byte;
    lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);

    add_byte(&current_code, &current_code_bits, byte, msb);
    source_bytes--;
  } while (source_bytes != 0);

  return 0;
}
