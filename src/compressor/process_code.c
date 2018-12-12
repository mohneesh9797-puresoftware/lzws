// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"
#include "ratio/main.h"

#include "process_code.h"
#include "utils.h"
#include "write.h"

static inline void read_next_symbol(lzws_compressor_state_t* state_ptr) {
  state_ptr->current_code = state_ptr->next_symbol;

  // It is possible to keep next symbol as is.
  // Algorithm won't touch next symbol without reinitialization.

  state_ptr->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;
}

static inline lzws_code_fast_t get_new_code(lzws_compressor_state_t* state_ptr) {
  if (lzws_compressor_is_dictionary_full(state_ptr)) {
    return LZWS_UNDEFINED_NEXT_CODE;
  }

  state_ptr->last_used_code++;
  lzws_code_fast_t new_code = state_ptr->last_used_code;

  lzws_code_fast_t first_code_for_next_code_bits = lzws_get_power_of_two(state_ptr->last_used_code_bits);
  if (new_code == first_code_for_next_code_bits) {
    state_ptr->last_used_code_bits++;
  }

  if (lzws_compressor_is_dictionary_full(state_ptr)) {
    // Dictionary become full.
    // We need to clear ratio now.
    lzws_compressor_clear_ratio(state_ptr);
  }

  return new_code;
}

lzws_result_t lzws_compressor_process_current_code(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result = lzws_compressor_write_current_code(state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    return result;
  }

  if (state_ptr->current_code == LZWS_CLEAR_CODE) {
    // We need to clear state after sending clear code.
    lzws_compressor_clear_state(state_ptr);
    read_next_symbol(state_ptr);

    return 0;
  }

  lzws_code_fast_t new_code = get_new_code(state_ptr);
  if (new_code != LZWS_UNDEFINED_NEXT_CODE) {
    // Dictionary is not full.
    lzws_compressor_save_next_code_to_dictionary_wrapper(state_ptr, state_ptr->next_symbol, new_code);
  }

  read_next_symbol(state_ptr);

  return 0;
}
