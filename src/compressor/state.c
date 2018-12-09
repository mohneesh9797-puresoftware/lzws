// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_STATE_C

#include "dictionary/wrapper.h"
#include "ratio/main.h"

#include "common.h"
#include "state.h"
#include "utils.h"

lzws_result_t lzws_compressor_get_initial_state(lzws_compressor_state_t** result, uint_fast8_t max_code_bits, bool block_mode, bool msb) {
  if (max_code_bits < LZWS_LOWEST_MAX_CODE_BITS || max_code_bits > LZWS_BIGGEST_MAX_CODE_BITS) {
    return LZWS_COMPRESSOR_INVALID_MAX_CODE_BITS;
  }

  lzws_compressor_state_t* state = malloc(sizeof(lzws_compressor_state_t));
  if (state == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  state->status = LZWS_COMPRESSOR_WRITE_HEADER;

  state->max_code_bits = max_code_bits;
  state->block_mode    = block_mode;
  state->msb           = msb;

  state->initial_used_code = lzws_get_initial_used_code(block_mode);
  state->max_code          = lzws_get_bit_mask(max_code_bits);

  state->last_used_code      = state->initial_used_code;
  state->last_used_code_bits = LZWS_LOWEST_MAX_CODE_BITS;

  // It is possible to keep "current_code" and "next_symbol" uninitialized.

  state->remainder      = 0;
  state->remainder_bits = 0;

  lzws_compressor_initialize_dictionary_wrapper(state);
  lzws_compressor_initialize_ratio(state);

  *result = state;

  return 0;
}

void lzws_compressor_clear_state(lzws_compressor_state_t* state) {
  state->last_used_code      = state->initial_used_code;
  state->last_used_code_bits = LZWS_LOWEST_MAX_CODE_BITS;

  lzws_compressor_clear_dictionary_wrapper(state);
  lzws_compressor_clear_ratio(state);
}

void lzws_compressor_free_state(lzws_compressor_state_t* state) {
  lzws_compressor_free_dictionary_wrapper(state);
  lzws_compressor_free_ratio(state);

  free(state);
}
