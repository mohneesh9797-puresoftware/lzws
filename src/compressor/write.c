// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../constants.h"
#include "../utils.h"

#include "common.h"
#include "write.h"

lzws_result_t lzws_compressor_write_current_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  uint8_t code_bits      = state->last_used_code_bits;
  uint8_t remainder_bits = state->remainder_bits;

  // Destination bytes will always be >= 1.
  uint8_t destination_bytes = (code_bits + remainder_bits) >> 3;

  if (*destination_length < destination_bytes) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_code_t code = state->current_code;
  uint8_t     byte;

  if (remainder_bits != 0) {
    uint8_t remainder      = state->remainder;
    uint8_t code_mask_bits = 8 - remainder_bits;

    // All bits of the mask are 1.
    uint8_t code_mask = LZWS_POWERS_OF_TWO[code_mask_bits] - 1;

    // Code bits here will always be > 8.
    byte = ((code & code_mask) << remainder_bits) | remainder;
    code >>= code_mask_bits;
    code_bits -= code_mask_bits;

    lzws_write_byte(destination, destination_length, byte);
  }

  while (code_bits >= 8) {
    byte = code & 0xff;
    code >>= 8;
    code_bits -= 8;

    lzws_write_byte(destination, destination_length, byte);
  }

  // We should keep current code as is.

  state->remainder      = code;
  state->remainder_bits = code_bits;

  return 0;
}

static inline lzws_result_t write_remainder(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  if (state->remainder_bits == 0) {
    return 0;
  }

  if (*destination_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_write_byte(destination, destination_length, state->remainder);

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
