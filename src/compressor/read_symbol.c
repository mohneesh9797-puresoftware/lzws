// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"
#include "ratio/main.h"

#include "common.h"
#include "read_symbol.h"
#include "utils.h"

lzws_result_t lzws_compressor_read_first_symbol(lzws_compressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  if (*source_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint_fast8_t symbol;
  lzws_compressor_read_byte(state_ptr, source_ptr, source_length_ptr, &symbol);

  state_ptr->current_code = symbol;
  state_ptr->status       = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;

  return 0;
}

lzws_result_t lzws_compressor_read_next_symbol(lzws_compressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  if (*source_length_ptr < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE;
  }

  // We want to clear dictionary when there is at least one symbol that will use new dictionary.
  // So we should check whether we need to clear dictionary before reading this symbol.
  //
  // Current code >= initial used code means that dictionary is still working on current source sequence.
  // We can't clear dictionary during source sequence.
  // We can ignore situation when current code equals clear code.
  // So we can compare current code with alphabet length.
  if (state_ptr->current_code < LZWS_ALPHABET_LENGTH && lzws_compressor_need_to_clear_by_ratio(state_ptr)) {
    state_ptr->next_symbol  = state_ptr->current_code;
    state_ptr->current_code = LZWS_CLEAR_CODE;

    state_ptr->status = LZWS_COMPRESSOR_PROCESS_CURRENT_CODE;

    return 0;
  }

  uint_fast8_t symbol;
  lzws_compressor_read_byte(state_ptr, source_ptr, source_length_ptr, &symbol);

  lzws_code_fast_t next_code = lzws_compressor_get_next_code_from_dictionary_wrapper(state_ptr, symbol);
  if (next_code != LZWS_UNDEFINED_NEXT_CODE) {
    // We found next code, it becomes new current code.
    state_ptr->current_code = next_code;

    // It is possible to keep next symbol as is.
    // Algorithm won't touch next symbol without reinitialization.

    // We don't need to change status, algorithm wants next symbol.
    return 0;
  }

  state_ptr->next_symbol = symbol;
  state_ptr->status      = LZWS_COMPRESSOR_PROCESS_CURRENT_CODE;

  return 0;
}
