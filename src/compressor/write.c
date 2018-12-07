// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "write.h"
#include "common.h"
#include "utils.h"

static inline uint8_t get_byte(lzws_code_t* code_ptr, uint8_t* code_bits_ptr, uint8_t remainder, uint8_t remainder_bits, bool msb) {
  lzws_code_t code      = *code_ptr;
  uint8_t     code_bits = *code_bits_ptr;

  uint8_t current_bits   = 8 - remainder_bits;
  uint8_t remaining_bits = code_bits - current_bits;

  uint8_t byte;

  if (msb) {
    // Taking first bits.
    byte = code >> remaining_bits;

    if (remainder_bits != 0) {
      // Remainder is sitting on the top.
      byte = (remainder << current_bits) | byte;
    }

    // Removing first bits.
    code &= LZWS_BIT_MASKS[remaining_bits];
  } else {
    // Taking last bits.
    byte = code & LZWS_BIT_MASKS[current_bits];

    if (remainder_bits != 0) {
      // Remainder is sitting on the bottom.
      byte = (byte << remainder_bits) | remainder;
    }

    // Removing last bits.
    code >>= current_bits;
  }

  *code_ptr      = code;
  *code_bits_ptr = remaining_bits;

  return byte;
}

lzws_result_t lzws_compressor_write_current_code(lzws_compressor_state_t* state, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  uint8_t code_bits      = state->last_used_code_bits;
  uint8_t remainder_bits = state->remainder_bits;

  // Code bits here will always be >= 8.
  // So destination bytes will always be >= 1.
  uint8_t destination_bytes = (code_bits + remainder_bits) >> 3;

  if (*destination_length_ptr < destination_bytes) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_code_t code      = state->current_code;
  uint8_t     remainder = state->remainder;
  bool        msb       = state->msb;

  uint8_t byte = get_byte(&code, &code_bits, remainder, remainder_bits, msb);
  lzws_compressor_write_byte(state, destination_ptr, destination_length_ptr, byte);

  while (code_bits >= 8) {
    byte = get_byte(&code, &code_bits, 0, 0, msb);
    lzws_compressor_write_byte(state, destination_ptr, destination_length_ptr, byte);
  }

  // We should keep current code as is.

  state->remainder      = code;
  state->remainder_bits = code_bits;

  return 0;
}

static inline lzws_result_t write_remainder(lzws_compressor_state_t* state, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  uint8_t remainder_bits = state->remainder_bits;
  if (remainder_bits == 0) {
    return 0;
  }

  if (*destination_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  // Remainder is left padded with zeroes by default.
  uint8_t remainder = state->remainder;

  if (state->msb) {
    // Adding right padding.
    remainder <<= 8 - remainder_bits;
  }

  lzws_compressor_write_byte(state, destination_ptr, destination_length_ptr, remainder);

  state->remainder      = 0;
  state->remainder_bits = 0;

  return 0;
}

lzws_result_t lzws_compressor_write_current_code_and_remainder(lzws_compressor_state_t* state, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result = lzws_compressor_write_current_code(state, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  return write_remainder(state, destination_ptr, destination_length_ptr);
}
