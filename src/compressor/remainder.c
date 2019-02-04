// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "alignment/wrapper.h"

#include "common.h"
#include "remainder.h"
#include "utils.h"

static inline lzws_result_t write_destination_remainder(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  uint_fast8_t destination_remainder_bit_length = state_ptr->destination_remainder_bit_length;
  if (destination_remainder_bit_length == 0) {
    return 0;
  }

  if (*destination_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_compressor_add_destination_byte_length_to_alignment_wrapper(state_ptr, 1);

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

lzws_result_t lzws_compressor_write_destination_remainder_before_read_next_symbol(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  lzws_result_t result = write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_WRITE_PADDING_ZEROES_BEFORE_READ_NEXT_SYMBOL;

  return 0;
}

lzws_result_t lzws_compressor_write_destination_remainder_before_current_code(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  lzws_result_t result = write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_WRITE_PADDING_ZEROES_BEFORE_CURRENT_CODE;

  return 0;
}

lzws_result_t lzws_compressor_flush_destination_remainder_before_current_code(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  lzws_result_t result = write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_COMPRESSOR_FLUSH_PADDING_ZEROES_BEFORE_CURRENT_CODE;

  return 0;
}

lzws_result_t lzws_compressor_flush_destination_remainder(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  // We don't need to change status after flushing destination remainder.
  // Algorithm has finished.
  return write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);
}
