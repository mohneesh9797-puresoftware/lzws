// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_STATE_C

#include "state.h"
#include "common.h"

lzws_result_t lzws_decompressor_get_initial_state(lzws_decompressor_state_t** result_state_ptr, bool msb) {
  lzws_decompressor_state_t* state_ptr = malloc(sizeof(lzws_decompressor_state_t));
  if (state_ptr == NULL) {
    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_HEADER;

  state_ptr->msb = msb;

  // lzws_decompressor_initialize_dictionary(state_ptr);

  *result_state_ptr = state_ptr;

  return 0;
}

void lzws_decompressor_free_state(lzws_decompressor_state_t* state_ptr) {
  // lzws_decompressor_free_dictionary(state_ptr);

  free(state_ptr);
}
