// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "read_code.h"
#include "common.h"
#include "utils.h"

static inline void add_byte(lzws_code_fast_t* code_ptr, uint_fast8_t* code_bits_ptr, uint_fast8_t byte, bool msb) {
  lzws_code_fast_t code      = *code_ptr;
  uint_fast8_t     code_bits = *code_bits_ptr;

  if (msb) {
    // Code is sitting on the top.
    code = (code << 8) | byte;
  } else {
    // Code is sitting on the bottom.
    code = (byte << code_bits) | code;
  }

  *code_ptr      = code;
  *code_bits_ptr = code_bits + 8;
}

static inline void add_last_byte(
  lzws_code_fast_t* code_ptr, uint_fast8_t code_bits, uint_fast8_t target_code_bits,
  uint_fast8_t byte, uint_fast8_t* remainder_ptr, uint_fast8_t* remainder_bits_ptr,
  bool msb) {
  //

  lzws_code_fast_t code = *code_ptr;

  uint_fast8_t code_part;
  uint_fast8_t code_part_bits = target_code_bits - code_bits;

  uint_fast8_t remainder;
  uint_fast8_t remainder_bits = 8 - code_part_bits;

  if (msb) {
    // Taking first bits from byte.
    code_part = byte >> remainder_bits;

    // Last bits of byte is a remainder.
    remainder = byte & lzws_get_bit_mask(remainder_bits);

    // Code is sitting on the top.
    code = (code << code_part_bits) | code_part;
  } else {
    // Taking last bits from byte.
    code_part = byte & lzws_get_bit_mask(code_part_bits);

    // First bits of byte is a remainder.
    remainder = byte >> code_part_bits;

    // Code is sitting on the bottom.
    code = (code_part << code_bits) | code;
  }

  *code_ptr           = code;
  *remainder_ptr      = remainder;
  *remainder_bits_ptr = remainder_bits;
}

lzws_result_t lzws_decompressor_read_current_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  uint_fast8_t target_code_bits      = state_ptr->last_used_code_bits;
  uint_fast8_t source_remainder_bits = state_ptr->source_remainder_bits;

  // Target code bits will always be >= 8.
  // Source remainder bits will always be <= 7.
  // So source bytes will always be >= 1.
  uint_fast8_t source_bytes = lzws_ceil_bits_to_bytes(target_code_bits - source_remainder_bits);
  if (*source_length_ptr < source_bytes) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  lzws_code_fast_t code      = state_ptr->source_remainder;
  uint_fast8_t     code_bits = source_remainder_bits;
  bool             msb       = state_ptr->msb;

  uint_fast8_t byte;

  while (source_bytes != 1) {
    lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);
    add_byte(&code, &code_bits, byte, msb);
    source_bytes--;
  }

  lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);
  add_last_byte(
    &code, code_bits, target_code_bits,
    byte, &state_ptr->source_remainder, &state_ptr->source_remainder_bits,
    msb);

  state_ptr->current_code = code;

  return 0;
}
