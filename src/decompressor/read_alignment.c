// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../log.h"

#include "common.h"
#include "read_alignment.h"
#include "utils.h"

lzws_result_t lzws_decompressor_read_padding_zeroes_for_alignment(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  uint_fast8_t byte;

  while (state_ptr->unaligned_source_byte_length != 0) {
    if (*source_length_ptr < 1) {
      return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
    }

    lzws_decompressor_update_unaligned_source_byte_length(state_ptr, 1);

    lzws_decompressor_read_byte(state_ptr, &byte, source_ptr, source_length_ptr);

    if (byte != 0) {
      if (!state_ptr->quiet) {
        LZWS_LOG_ERROR("received non zero padding byte")
      }

      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }
  }

  if (state_ptr->unaligned_by_code_bit_length > state_ptr->last_used_code_bit_length) {
    // Alignment was written after clear code.
    // Now we need to process first code.

    state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_FIRST_CODE;
  }
  else {
    // Alignment was written after increasing last used code bit length.
    // Now we need to process next code.

    state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_NEXT_CODE;
  }

  state_ptr->unaligned_by_code_bit_length = state_ptr->last_used_code_bit_length;

  return 0;
}
