// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_STATE_C

#include "dictionary/wrapper.h"
#include "ratio/main.h"

#include "../log.h"
#include "../utils.h"

#include "common.h"
#include "state.h"

lzws_result_t lzws_compressor_get_initial_state(
  lzws_compressor_state_t** result_state_ptr,
  uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool quiet, bool unaligned_bit_groups)
{
  if (max_code_bit_length < LZWS_LOWEST_MAX_CODE_BIT_LENGTH || max_code_bit_length > LZWS_BIGGEST_MAX_CODE_BIT_LENGTH) {
    if (!quiet) {
      LZWS_LOG_ERROR("invalid max code bit length: %u", max_code_bit_length)
    }

    return LZWS_COMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH;
  }

  size_t                   state_size = sizeof(lzws_compressor_state_t);
  lzws_compressor_state_t* state_ptr  = malloc(state_size);

  if (state_ptr == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, state size: %zu", state_size)
    }

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  lzws_code_fast_t first_free_code = lzws_get_first_free_code(block_mode);

  state_ptr->status = LZWS_COMPRESSOR_WRITE_HEADER;

  state_ptr->max_code_bit_length  = max_code_bit_length;
  state_ptr->block_mode           = block_mode;
  state_ptr->msb                  = msb;
  state_ptr->quiet                = quiet;
  state_ptr->unaligned_bit_groups = unaligned_bit_groups;

  state_ptr->first_free_code = first_free_code;
  state_ptr->max_code        = lzws_get_mask_for_last_bits(max_code_bit_length);

  state_ptr->last_used_code            = first_free_code - 1;
  state_ptr->last_used_max_code        = lzws_get_mask_for_last_bits(LZWS_LOWEST_MAX_CODE_BIT_LENGTH);
  state_ptr->last_used_code_bit_length = LZWS_LOWEST_MAX_CODE_BIT_LENGTH;

  // It is possible to keep current code and next symbol uninitialized.

  state_ptr->destination_remainder            = 0;
  state_ptr->destination_remainder_bit_length = 0;

  if (!unaligned_bit_groups) {
    state_ptr->unaligned_by_code_bit_length      = LZWS_LOWEST_MAX_CODE_BIT_LENGTH;
    state_ptr->unaligned_destination_byte_length = 0;
  }

  lzws_compressor_initialize_dictionary_wrapper(state_ptr);
  lzws_compressor_initialize_ratio(state_ptr);

  *result_state_ptr = state_ptr;

  return 0;
}

void lzws_compressor_clear_state(lzws_compressor_state_t* state_ptr)
{
  state_ptr->last_used_code            = state_ptr->first_free_code - 1;
  state_ptr->last_used_max_code        = lzws_get_mask_for_last_bits(LZWS_LOWEST_MAX_CODE_BIT_LENGTH);
  state_ptr->last_used_code_bit_length = LZWS_LOWEST_MAX_CODE_BIT_LENGTH;

  lzws_compressor_clear_dictionary_wrapper(state_ptr);
  lzws_compressor_clear_ratio(state_ptr);
}

void lzws_compressor_free_state(lzws_compressor_state_t* state_ptr)
{
  lzws_compressor_free_dictionary_wrapper(state_ptr);
  lzws_compressor_free_ratio(state_ptr);

  free(state_ptr);
}
