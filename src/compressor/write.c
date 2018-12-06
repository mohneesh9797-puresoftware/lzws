// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "write.h"
#include "common.h"
#include "utils.h"

static inline uint8_t get_byte(bool msb, uint8_t remainder, uint8_t remainder_bits, lzws_code_t* code_ptr, uint8_t* code_bits_ptr) {
  lzws_code_t code      = *code_ptr;
  uint8_t     code_bits = *code_bits_ptr;

  uint8_t     code_mask_bits = 8 - remainder_bits;
  lzws_code_t code_mask      = LZWS_POWERS_OF_TWO[code_mask_bits] - 1;

  uint8_t remaining_code;
  uint8_t remaining_code_bits = code_bits - code_mask_bits;

  uint8_t byte;
  if (msb) {
    // Taking first code mask bits.
    byte = code & (code_mask << remaining_code_bits);

    if (remainder_bits != 0) {
      // Remainder is sitting on the top.
      byte = (remainder << code_mask_bits) | byte;
    }

    // Removing first code mask bits.
    lzws_code_t remaining_code_mask = LZWS_POWERS_OF_TWO[remaining_code_bits] - 1;
    remaining_code                  = code & remaining_code_mask;
  } else {
    // Taking last code mask bits.
    byte = code & code_mask;

    if (remainder_bits != 0) {
      // Remainder is sitting on the bottom.
      byte = (byte << remainder_bits) | remainder;
    }

    // Removing last code mask bits.
    remaining_code = code >> code_mask_bits;
  }

  *code_ptr      = remaining_code;
  *code_bits_ptr = remaining_code_bits;

  return byte;
}

lzws_result_t lzws_compressor_write_current_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  uint8_t code_bits      = state->last_used_code_bits;
  uint8_t remainder_bits = state->remainder_bits;

  // Code bits here will always be >= 8.
  // So destination bytes will always be >= 1.
  uint8_t destination_bytes = (code_bits + remainder_bits) >> 3;

  if (*destination_length < destination_bytes) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  bool        msb       = state->msb;
  lzws_code_t code      = state->current_code;
  uint8_t     remainder = state->remainder;

  uint8_t byte = get_byte(msb, remainder, remainder_bits, &code, &code_bits);
  lzws_compressor_write_byte(state, destination, destination_length, byte);

  while (code_bits >= 8) {
    byte = get_byte(msb, 0, 0, &code, &code_bits);
    lzws_compressor_write_byte(state, destination, destination_length, byte);
  }

  // We should keep current code as is.

  state->remainder      = code;
  state->remainder_bits = code_bits;

  return 0;
}

static inline lzws_result_t write_remainder(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  uint8_t remainder_bits = state->remainder_bits;
  if (remainder_bits == 0) {
    return 0;
  }

  if (*destination_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  uint8_t remainder = state->remainder;
  if (state->msb) {
    // Right padding remainder with zeroes.
    remainder <<= 8 - remainder_bits;
  }

  lzws_compressor_write_byte(state, destination, destination_length, remainder);

  state->remainder      = 0;
  state->remainder_bits = 0;

  return 0;
}

lzws_result_t lzws_compressor_write_current_code_and_remainder(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result = lzws_compressor_write_current_code(state, destination, destination_length);
  if (result != 0) {
    return result;
  }

  return write_remainder(state, destination, destination_length);
}
