// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "alignment.h"
#include "common.h"
#include "utils.h"

lzws_result_t lzws_compressor_write_padding_zeroes_for_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  uint_fast8_t byte = 0;

  while (state_ptr->unaligned_destination_byte_length != 0) {
    if (*destination_length_ptr < 1) {
      return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
    }

    lzws_compressor_update_unaligned_destination_byte_length(state_ptr, 1);

    lzws_compressor_write_byte(state_ptr, byte, destination_ptr, destination_length_ptr);
  }

  state_ptr->unaligned_by_code_bit_length = state_ptr->last_used_code_bit_length;

  // Alignment was written after clear code or after increasing of last used code bit length.
  // It doesn't matter, now we need to read next symbol.

  state_ptr->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;

  return 0;
}
