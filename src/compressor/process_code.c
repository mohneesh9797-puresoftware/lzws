// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../constants.h"

#include "dictionary/wrapper.h"

#include "process_code.h"
#include "write.h"

static inline lzws_code_t get_new_code(lzws_compressor_state_t* state) {
  if (lzws_compressor_is_dictionary_full(state)) {
    // Dictionary is full.
    return LZWS_UNDEFINED_NEXT_CODE;
  }

  state->last_used_code++;
  lzws_code_t new_code = state->last_used_code;

  lzws_code_t first_code_for_next_code_bits = LZWS_POWERS_OF_TWO[state->last_used_code_bits];
  if (new_code == first_code_for_next_code_bits) {
    state->last_used_code_bits++;
  }

  return new_code;
}

static inline void process_next_symbol(lzws_compressor_state_t* state) {
  lzws_code_t new_code = get_new_code(state);
  if (new_code != LZWS_UNDEFINED_NEXT_CODE) {
    // Dictionary is not full.
    lzws_compressor_save_next_code_to_dictionary_wrapper(state, state->next_symbol, new_code);
  }

  state->current_code = state->next_symbol;

  // It is possible to keep next symbol as is.
  // Algorithm won't touch next symbol without reinitialization.

  state->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;
}

lzws_result_t lzws_compressor_process_current_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result = lzws_compressor_write_current_code(state, destination, destination_length);
  if (result != 0) {
    return result;
  }

  process_next_symbol(state);

  return 0;
}
