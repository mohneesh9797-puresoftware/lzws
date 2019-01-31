// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "alignment/wrapper.h"

#include "../utils.h"

#include "code.h"
#include "common.h"
#include "utils.h"

static inline uint_fast8_t get_byte(lzws_code_fast_t* code_ptr, uint_fast8_t* code_bit_length_ptr, bool msb)
{
  lzws_code_fast_t code            = *code_ptr;
  uint_fast8_t     code_bit_length = *code_bit_length_ptr;

  uint_fast8_t remaining_code_bit_length = code_bit_length - 8;

  uint_fast8_t byte;

  if (msb) {
    // Taking first bits from code.
    byte = code >> remaining_code_bit_length;

    // Removing first bits from code.
    code &= lzws_get_mask_for_last_bits(remaining_code_bit_length);
  }
  else {
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
  bool msb)
{
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
  }
  else {
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

lzws_result_t lzws_compressor_write_code(lzws_compressor_state_t* state_ptr, lzws_code_fast_t code, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  uint_fast8_t code_bit_length                  = state_ptr->last_used_code_bit_length;
  uint_fast8_t destination_remainder_bit_length = state_ptr->destination_remainder_bit_length;

  // Code bit length will always be >= 8.
  // Destination remainder bit length will always be <= 7.
  // So destination byte length will always be >= 1.

  uint_fast8_t destination_byte_length = lzws_floor_bit_length_to_byte_length(code_bit_length + destination_remainder_bit_length);

  if (*destination_length_ptr < destination_byte_length) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_compressor_add_destination_byte_length_to_alignment_wrapper(state_ptr, destination_byte_length);

  uint_fast8_t destination_remainder = state_ptr->destination_remainder;
  bool         msb                   = state_ptr->msb;

  uint_fast8_t byte = get_byte_with_remainder(&code, &code_bit_length, destination_remainder, destination_remainder_bit_length, msb);
  lzws_compressor_write_byte(state_ptr, byte, destination_ptr, destination_length_ptr);

  destination_byte_length--;

  while (destination_byte_length != 0) {
    byte = get_byte(&code, &code_bit_length, msb);
    lzws_compressor_write_byte(state_ptr, byte, destination_ptr, destination_length_ptr);

    destination_byte_length--;
  }

  state_ptr->destination_remainder            = code;
  state_ptr->destination_remainder_bit_length = code_bit_length;

  return 0;
}
