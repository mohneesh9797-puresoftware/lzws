// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../log.h"

#include "common.h"
#include "remainder.h"

lzws_result_t lzws_decompressor_verify_empty_remainder(lzws_decompressor_state_t* state_ptr)
{
  uint_fast8_t remainder            = state_ptr->remainder;
  uint_fast8_t remainder_bit_length = state_ptr->remainder_bit_length;

  if (remainder != 0 && remainder_bit_length != 0) {
    if (!state_ptr->quiet) {
      LZWS_LOG_ERROR("remainder is not empty, value: %u, bit length: %u", remainder, remainder_bit_length)
    }

    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }

  return 0;
}

lzws_result_t lzws_decompressor_verify_empty_remainder_for_alignment(lzws_decompressor_state_t* state_ptr)
{
  lzws_result_t result = lzws_decompressor_verify_empty_remainder(state_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_PADDING_ZEROES_FOR_ALIGNMENT;

  return 0;
}
