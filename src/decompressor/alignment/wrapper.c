// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_ALIGNMENT_WRAPPER_C

#include "../../log.h"
#include "../common.h"
#include "../utils.h"

#include "wrapper.h"

lzws_result_t lzws_decompressor_read_padding_zeroes_for_alignment_wrapper(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_decompressor_alignment_t* alignment_ptr = &state_ptr->alignment;

  uint_fast8_t byte;

  while (alignment_ptr->source_byte_length != 0) {
    if (*source_length_ptr < 1) {
      return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
    }

    lzws_decompressor_add_source_byte_length_to_alignment(alignment_ptr, 1);

    lzws_decompressor_read_byte(state_ptr, &byte, source_ptr, source_length_ptr);

    if (byte != 0) {
      if (!state_ptr->quiet) {
        LZWS_LOG_ERROR("received non zero padding byte")
      }

      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }
  }

  uint_fast8_t last_used_code_bit_length = state_ptr->last_used_code_bit_length;
  if (alignment_ptr->last_used_code_bit_length > last_used_code_bit_length) {
    // Alignment was read after current code (clear code).
    // Now we need to process first code.
    state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_FIRST_CODE;
  }
  else {
    // Alignment was read before current code (after increasing last used code bit length).
    // Now we need to process next code.
    state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_NEXT_CODE;
  }

  alignment_ptr->last_used_code_bit_length = last_used_code_bit_length;

  return 0;
}
