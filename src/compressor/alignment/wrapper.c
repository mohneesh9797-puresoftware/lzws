// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_ALIGNMENT_WRAPPER_C

#include "../common.h"
#include "../utils.h"

#include "wrapper.h"

lzws_result_t lzws_compressor_write_padding_zeroes_for_alignment_wrapper(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  lzws_compressor_alignment_t* alignment_ptr = &state_ptr->alignment;

  uint_fast8_t byte = 0;

  while (alignment_ptr->destination_byte_length != 0) {
    if (*destination_length_ptr < 1) {
      return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
    }

    lzws_compressor_add_destination_byte_length_to_alignment(alignment_ptr, 1);

    lzws_compressor_write_byte(state_ptr, byte, destination_ptr, destination_length_ptr);
  }

  uint_fast8_t last_used_code_bit_length = state_ptr->last_used_code_bit_length;

  if (alignment_ptr->last_used_code_bit_length > last_used_code_bit_length) {
    // Alignment was written after current code (clear code).
    // So we need to read next symbol.

    state_ptr->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;
  }
  else {
    // Alignment was written before current code (after increasing last used code bit length).
    // So we need to write current code.

    state_ptr->status = LZWS_COMPRESSOR_WRITE_CURRENT_CODE;
  }

  lzws_compressor_set_last_used_code_bit_length_to_alignment(alignment_ptr, last_used_code_bit_length);

  return 0;
}
