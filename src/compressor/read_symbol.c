// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"

#include "common.h"
#include "read_symbol.h"
#include "utils.h"

#define READ_SYMBOL                           \
  if (*source_length < 1) {                   \
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE; \
  }                                           \
                                              \
  uint8_t symbol;                             \
  lzws_compressor_read_byte(state, source, source_length, &symbol);

lzws_result_t lzws_compressor_read_first_symbol(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length) {
  READ_SYMBOL

  state->current_code = symbol;
  state->status       = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;

  return 0;
}

lzws_result_t lzws_compressor_read_next_symbol(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length) {
  READ_SYMBOL

  lzws_code_t next_code = lzws_compressor_get_next_code_from_dictionary_wrapper(state, symbol);
  if (next_code != LZWS_UNDEFINED_NEXT_CODE) {
    // We found next code, it becomes new current code.
    state->current_code = next_code;

    // We don't need to change status, algorithm wants next symbol.
    return 0;
  }

  state->next_symbol = symbol;
  state->status      = LZWS_COMPRESSOR_PROCESS_CURRENT_CODE;

  return 0;
}
