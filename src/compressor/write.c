// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../utils.h"

#include "common.h"
#include "utils.h"
#include "write.h"

static inline uint_fast8_t get_byte(lzws_code_fast_t* code_ptr, uint_fast8_t* code_bits_ptr, bool msb) {
  lzws_code_fast_t code      = *code_ptr;
  uint_fast8_t     code_bits = *code_bits_ptr;

  uint_fast8_t remaining_code_bits = code_bits - 8;

  uint_fast8_t byte;

  if (msb) {
    // Taking first bits from code.
    byte = code >> remaining_code_bits;

    // Removing first bits from code.
    code &= lzws_get_bit_mask(remaining_code_bits);
  } else {
    // Taking last bits from code.
    byte = code & 0xff;

    // Removing last bits from code.
    code >>= 8;
  }

  *code_ptr      = code;
  *code_bits_ptr = remaining_code_bits;

  return byte;
}

static inline uint_fast8_t get_byte_with_remainder(lzws_code_fast_t* code_ptr, uint_fast8_t* code_bits_ptr, uint_fast8_t destination_remainder, uint_fast8_t destination_remainder_bits, bool msb) {
  if (destination_remainder_bits == 0) {
    return get_byte(code_ptr, code_bits_ptr, msb);
  }

  lzws_code_fast_t code      = *code_ptr;
  uint_fast8_t     code_bits = *code_bits_ptr;

  uint_fast8_t current_code_bits   = 8 - destination_remainder_bits;
  uint_fast8_t remaining_code_bits = code_bits - current_code_bits;

  uint_fast8_t byte;

  if (msb) {
    // Taking first bits from code.
    byte = code >> remaining_code_bits;

    // Destination remainder is sitting on the top.
    byte = (destination_remainder << current_code_bits) | byte;

    // Removing first bits from code.
    code &= lzws_get_bit_mask(remaining_code_bits);
  } else {
    // Taking last bits from code.
    byte = code & lzws_get_bit_mask(current_code_bits);

    // Destination remainder is sitting on the bottom.
    byte = (byte << destination_remainder_bits) | destination_remainder;

    // Removing last bits from code.
    code >>= current_code_bits;
  }

  *code_ptr      = code;
  *code_bits_ptr = remaining_code_bits;

  return byte;
}

lzws_result_t lzws_compressor_write_current_code(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  uint_fast8_t code_bits                  = state_ptr->last_used_code_bits;
  uint_fast8_t destination_remainder_bits = state_ptr->destination_remainder_bits;

  // Code bits will always be >= 8.
  // Destination remainder bits will always be <= 7.
  // So destination bytes will always be >= 1.
  uint_fast8_t destination_bytes = lzws_floor_bits_to_bytes(code_bits + destination_remainder_bits);
  if (*destination_length_ptr < destination_bytes) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_code_fast_t code                  = state_ptr->current_code;
  uint_fast8_t     destination_remainder = state_ptr->destination_remainder;
  bool             msb                   = state_ptr->msb;

  uint_fast8_t byte = get_byte_with_remainder(&code, &code_bits, destination_remainder, destination_remainder_bits, msb);
  lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, byte);

  while (destination_bytes != 1) {
    byte = get_byte(&code, &code_bits, msb);
    lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, byte);

    destination_bytes--;
  }

  // We should keep current code as is.

  state_ptr->destination_remainder      = code;
  state_ptr->destination_remainder_bits = code_bits;

  return 0;
}

lzws_result_t lzws_compressor_flush_current_code(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result = lzws_compressor_write_current_code(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_WRITE_DESTINATION_REMAINDER;

  return 0;
}

lzws_result_t lzws_compressor_write_destination_remainder(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  uint_fast8_t destination_remainder_bits = state_ptr->destination_remainder_bits;
  if (destination_remainder_bits == 0) {
    return 0;
  }

  if (*destination_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  // Destination remainder is left padded with zeroes by default.
  uint_fast8_t destination_remainder = state_ptr->destination_remainder;

  if (state_ptr->msb) {
    // Adding right padding.
    destination_remainder <<= 8 - destination_remainder_bits;
  }

  lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, destination_remainder);

  state_ptr->destination_remainder      = 0;
  state_ptr->destination_remainder_bits = 0;

  return 0;
}

lzws_result_t lzws_compressor_write_destination_remainder_for_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result = lzws_compressor_write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_WRITE_ALIGNMENT;

  return 0;
}

lzws_result_t lzws_compressor_write_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  // TODO

  state_ptr->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;

  return 0;
}
