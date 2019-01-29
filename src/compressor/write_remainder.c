// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "write_remainder.h"
#include "common.h"
#include "utils.h"

lzws_result_t lzws_compressor_write_destination_remainder(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  uint_fast8_t destination_remainder_bit_length = state_ptr->destination_remainder_bit_length;

  if (destination_remainder_bit_length == 0) {
    return 0;
  }

  if (*destination_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  if (!state_ptr->unaligned_bit_groups) {
    lzws_compressor_update_unaligned_destination_byte_length(state_ptr, 1);
  }

  // Destination remainder is left padded with zeroes by default.
  uint_fast8_t destination_remainder = state_ptr->destination_remainder;

  if (state_ptr->msb) {
    // Adding right padding.
    destination_remainder <<= 8 - destination_remainder_bit_length;
  }

  lzws_compressor_write_byte(state_ptr, destination_remainder, destination_ptr, destination_length_ptr);

  state_ptr->destination_remainder            = 0;
  state_ptr->destination_remainder_bit_length = 0;

  return 0;
}

lzws_result_t lzws_compressor_write_destination_remainder_for_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  lzws_result_t result = lzws_compressor_write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_WRITE_PADDING_ZEROES_FOR_ALIGNMENT;

  return 0;
}
