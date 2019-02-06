// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_ALIGNMENT_WRAPPER_C

#include "../../log.h"
#include "../common.h"
#include "../utils.h"

#include "wrapper.h"

static inline lzws_result_t read_alignment(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_decompressor_alignment_t* alignment_ptr = &state_ptr->alignment;

  uint_fast8_t byte;

  while (lzws_decompressor_need_to_read_alignment_byte(alignment_ptr)) {
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

  lzws_decompressor_reset_alignment_after_reading(alignment_ptr, state_ptr->last_used_code_bit_length);

  return 0;
}

lzws_result_t lzws_decompressor_read_alignment_before_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_result_t result = read_alignment(state_ptr, source_ptr, source_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_FIRST_CODE;

  return 0;
}

lzws_result_t lzws_decompressor_read_alignment_before_next_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_result_t result = read_alignment(state_ptr, source_ptr, source_length_ptr);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_NEXT_CODE;

  return 0;
}
