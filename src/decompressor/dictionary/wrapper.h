// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_DICTIONARY_WRAPPER_H)
#define LZWS_DECOMPRESSOR_DICTIONARY_WRAPPER_H

#include "../state.h"

#include "main.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_DICTIONARY_WRAPPER_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_decompressor_initialize_dictionary_wrapper(lzws_decompressor_state_t* state_ptr) {
  lzws_decompressor_initialize_dictionary(&state_ptr->dictionary);
}

LZWS_INLINE lzws_result_t lzws_decompressor_allocate_dictionary_wrapper(lzws_decompressor_state_t* state_ptr) {
  lzws_result_t result = lzws_decompressor_allocate_dictionary(&state_ptr->dictionary, state_ptr->initial_used_code, state_ptr->max_code_bits);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_READ_FIRST_CODE;

  return 0;
}

LZWS_INLINE void lzws_decompressor_clear_dictionary_wrapper(lzws_decompressor_state_t* state_ptr) {
  lzws_decompressor_clear_dictionary(&state_ptr->dictionary, state_ptr->max_code_bits);
}

LZWS_INLINE void lzws_decompressor_free_dictionary_wrapper(lzws_decompressor_state_t* state_ptr) {
  lzws_decompressor_free_dictionary(&state_ptr->dictionary);
}

#endif // LZWS_DECOMPRESSOR_DICTIONARY_WRAPPER_H
