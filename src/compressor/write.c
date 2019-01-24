// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../utils.h"

#include "common.h"
#include "utils.h"
#include "write.h"

static inline uint_fast8_t get_byte(lzws_code_fast_t* code_ptr, uint_fast8_t* code_bit_length_ptr, bool msb) {
  lzws_code_fast_t code            = *code_ptr;
  uint_fast8_t     code_bit_length = *code_bit_length_ptr;

  uint_fast8_t remaining_code_bit_length = code_bit_length - 8;

  uint_fast8_t byte;

  if (msb) {
    // Taking first bits from code.
    byte = code >> remaining_code_bit_length;

    // Removing first bits from code.
    code &= lzws_get_mask_for_last_bits(remaining_code_bit_length);
  } else {
    // Taking last bits from code.
    byte = code & 0xff;

    // Removing last bits from code.
    code >>= 8;
  }

  *code_ptr            = code;
  *code_bit_length_ptr = remaining_code_bit_length;

  return byte;
}

static inline uint_fast8_t get_byte_with_remainder(
  lzws_code_fast_t* code_ptr, uint_fast8_t* code_bit_length_ptr,
  uint_fast8_t destination_remainder, uint_fast8_t destination_remainder_bit_length,
  bool msb) {
  //

  if (destination_remainder_bit_length == 0) {
    return get_byte(code_ptr, code_bit_length_ptr, msb);
  }

  lzws_code_fast_t code            = *code_ptr;
  uint_fast8_t     code_bit_length = *code_bit_length_ptr;

  uint_fast8_t current_code_bit_length   = 8 - destination_remainder_bit_length;
  uint_fast8_t remaining_code_bit_length = code_bit_length - current_code_bit_length;

  uint_fast8_t byte;

  if (msb) {
    // Taking first bits from code.
    byte = code >> remaining_code_bit_length;

    // Destination remainder is sitting on the top.
    byte = (destination_remainder << current_code_bit_length) | byte;

    // Removing first bits from code.
    code &= lzws_get_mask_for_last_bits(remaining_code_bit_length);
  } else {
    // Taking last bits from code.
    byte = code & lzws_get_mask_for_last_bits(current_code_bit_length);

    // Destination remainder is sitting on the bottom.
    byte = (byte << destination_remainder_bit_length) | destination_remainder;

    // Removing last bits from code.
    code >>= current_code_bit_length;
  }

  *code_ptr            = code;
  *code_bit_length_ptr = remaining_code_bit_length;

  return byte;
}

// Destination byte length can be aligned by unaligned by code bit length * 8.

static inline void update_unaligned_destination_byte_length(lzws_compressor_state_t* state_ptr, uint_fast8_t destination_byte_length) {
  state_ptr->unaligned_destination_byte_length = (state_ptr->unaligned_destination_byte_length + destination_byte_length) % state_ptr->unaligned_by_code_bit_length;
}

lzws_result_t lzws_compressor_write_current_code(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  uint_fast8_t code_bit_length                  = state_ptr->last_used_code_bit_length;
  uint_fast8_t destination_remainder_bit_length = state_ptr->destination_remainder_bit_length;

  // Code bit length will always be >= 8.
  // Destination remainder bit length will always be <= 7.
  // So destination byte length will always be >= 1.
  uint_fast8_t destination_byte_length = lzws_floor_bit_length_to_byte_length(code_bit_length + destination_remainder_bit_length);
  if (*destination_length_ptr < destination_byte_length) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_code_fast_t code                  = state_ptr->current_code;
  uint_fast8_t     destination_remainder = state_ptr->destination_remainder;
  bool             msb                   = state_ptr->msb;

  uint_fast8_t byte = get_byte_with_remainder(&code, &code_bit_length, destination_remainder, destination_remainder_bit_length, msb);
  lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, byte);

  while (destination_byte_length != 1) {
    byte = get_byte(&code, &code_bit_length, msb);
    lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, byte);

    destination_byte_length--;
  }

  // We should keep current code as is.

  state_ptr->destination_remainder            = code;
  state_ptr->destination_remainder_bit_length = code_bit_length;

  if (!state_ptr->unaligned) {
    update_unaligned_destination_byte_length(state_ptr, destination_byte_length);
  }

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
  uint_fast8_t destination_remainder_bit_length = state_ptr->destination_remainder_bit_length;
  if (destination_remainder_bit_length == 0) {
    return 0;
  }

  if (*destination_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  // Destination remainder is left padded with zeroes by default.
  uint_fast8_t destination_remainder = state_ptr->destination_remainder;

  if (state_ptr->msb) {
    // Adding right padding.
    destination_remainder <<= 8 - destination_remainder_bit_length;
  }

  lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, destination_remainder);

  state_ptr->destination_remainder            = 0;
  state_ptr->destination_remainder_bit_length = 0;

  if (!state_ptr->unaligned) {
    update_unaligned_destination_byte_length(state_ptr, 1);
  }

  return 0;
}

lzws_result_t lzws_compressor_write_destination_remainder_for_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result = lzws_compressor_write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_WRITE_PADDING_ZEROES_FOR_ALIGNMENT;

  return 0;
}

lzws_result_t lzws_compressor_write_padding_zeroes_for_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  uint_fast8_t byte = 0;

  while (state_ptr->unaligned_destination_byte_length != 0) {
    if (*destination_length_ptr < 1) {
      return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
    }

    lzws_compressor_write_byte(state_ptr, destination_ptr, destination_length_ptr, byte);
    update_unaligned_destination_byte_length(state_ptr, 1);
  }

  state_ptr->unaligned_by_code_bit_length = state_ptr->last_used_code_bit_length;

  state_ptr->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;

  return 0;
}
