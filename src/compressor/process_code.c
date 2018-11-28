// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../constants.h"

#include "dictionary/wrapper.h"
#include "ratio/main.h"

#include "process_code.h"
#include "write.h"

static inline void continue_with_next_symbol(lzws_compressor_state_t* state) {
  state->current_code = state->next_symbol;

  // It is possible to keep next symbol as is.
  // Algorithm won't touch next symbol without reinitialization.

  state->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;
}

static inline lzws_code_t get_new_code(lzws_compressor_state_t* state) {
  if (lzws_compressor_is_dictionary_full(state)) {
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

static inline void process_current_code_and_next_symbol(lzws_compressor_state_t* state) {
  if (state->current_code == LZWS_CLEAR_CODE) {
    // We need to clear state after sending clear code.
    lzws_compressor_clear_state(state);
    continue_with_next_symbol(state);
    return;
  }

  if (lzws_compressor_need_to_clear_by_ratio(state)) {
    state->current_code = LZWS_CLEAR_CODE;

    // We don't need to change status, algorithm wants to process current code.
    return;
  }

  lzws_code_t new_code = get_new_code(state);
  if (new_code != LZWS_UNDEFINED_NEXT_CODE) {
    // Dictionary is not full.
    lzws_compressor_save_next_code_to_dictionary_wrapper(state, state->next_symbol, new_code);
  }

  continue_with_next_symbol(state);
}

lzws_result_t lzws_compressor_process_current_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result = lzws_compressor_write_current_code(state, destination, destination_length);
  if (result != 0) {
    return result;
  }

  process_current_code_and_next_symbol(state);

  return 0;
}
