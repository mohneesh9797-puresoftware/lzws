// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_STATE_C

#include "dictionary/wrapper.h"

#include "common.h"
#include "state.h"

lzws_result_t lzws_decompressor_get_initial_state(lzws_decompressor_state_t** result_state_ptr, bool msb) {
  lzws_decompressor_state_t* state_ptr = malloc(sizeof(lzws_decompressor_state_t));
  if (state_ptr == NULL) {
    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_HEADER;

  state_ptr->msb = msb;

  state_ptr->source_remainder      = 0;
  state_ptr->source_remainder_bits = 0;

  // Other data will be initialized during header.

  lzws_decompressor_initialize_dictionary_wrapper(state_ptr);

  *result_state_ptr = state_ptr;

  return 0;
}

void lzws_decompressor_free_state(lzws_decompressor_state_t* state_ptr) {
  lzws_decompressor_free_dictionary_wrapper(state_ptr);

  free(state_ptr);
}
