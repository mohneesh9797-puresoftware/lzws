// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_STATE_H)
#define LZWS_COMPRESSOR_DICTIONARY_STATE_H

#include "../../macro.h"
#include "../state.h"

#include "common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_STATE_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_dictionary_in_state(lzws_compressor_state_t* state) {
  lzws_compressor_initialize_dictionary(&state->dictionary, state->initial_used_code);
}

LZWS_INLINE lzws_result_t lzws_compressor_allocate_dictionary_in_state(lzws_compressor_state_t* state) {
  lzws_result_t result = lzws_compressor_allocate_dictionary(&state->dictionary, state->max_code_bits);
  if (result != 0) {
    return result;
  }

  state->status = LZWS_COMPRESSOR_READ_FIRST_SYMBOL;

  return 0;
}

LZWS_INLINE void lzws_compressor_clear_dictionary_in_state(lzws_compressor_state_t* LZWS_UNUSED(state)) {
  // TODO
}

LZWS_INLINE lzws_code_t lzws_compressor_get_next_code_from_dictionary_in_state(lzws_compressor_state_t* state, uint8_t symbol) {
  return lzws_compressor_get_next_code_from_dictionary(&state->dictionary, state->current_code, symbol);
}

LZWS_INLINE void lzws_compressor_save_next_code_to_dictionary_in_state(lzws_compressor_state_t* state, uint8_t symbol, lzws_code_t code) {
  lzws_compressor_save_next_code_to_dictionary(&state->dictionary, state->current_code, symbol, code);
}

LZWS_INLINE void lzws_compressor_free_dictionary_in_state(lzws_compressor_state_t* state) {
  lzws_compressor_free_dictionary(&state->dictionary);
}

#endif // LZWS_COMPRESSOR_DICTIONARY_STATE_H
