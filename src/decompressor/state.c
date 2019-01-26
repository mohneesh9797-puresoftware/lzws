// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_STATE_C

#include "dictionary/wrapper.h"

#include "../log.h"
#include "../utils.h"

#include "common.h"
#include "state.h"

lzws_result_t lzws_decompressor_get_initial_state(lzws_decompressor_state_t** result_state_ptr, bool msb, bool quiet, bool unaligned_bit_groups)
{
  size_t                     state_size = sizeof(lzws_decompressor_state_t);
  lzws_decompressor_state_t* state_ptr  = malloc(state_size);

  if (state_ptr == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, state size: %zu", state_size)
    }

    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_HEADER;

  state_ptr->msb                  = msb;
  state_ptr->quiet                = quiet;
  state_ptr->unaligned_bit_groups = unaligned_bit_groups;

  state_ptr->source_remainder            = 0;
  state_ptr->source_remainder_bit_length = 0;

  // Other data will be initialized during reading of header.

  lzws_decompressor_initialize_dictionary_wrapper(state_ptr);

  *result_state_ptr = state_ptr;

  return 0;
}

void lzws_decompressor_clear_state(lzws_decompressor_state_t* state_ptr)
{
  state_ptr->last_used_code            = state_ptr->initial_used_code;
  state_ptr->last_used_max_code        = lzws_get_mask_for_last_bits(LZWS_LOWEST_MAX_CODE_BIT_LENGTH);
  state_ptr->last_used_code_bit_length = LZWS_LOWEST_MAX_CODE_BIT_LENGTH;

  // We don't need to clear dictionary.
}

void lzws_decompressor_free_state(lzws_decompressor_state_t* state_ptr)
{
  lzws_decompressor_free_dictionary_wrapper(state_ptr);

  free(state_ptr);
}
