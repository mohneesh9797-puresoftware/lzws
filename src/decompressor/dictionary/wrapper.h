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

LZWS_INLINE size_t lzws_decompressor_get_total_codes_length(lzws_decompressor_state_t* state_ptr)
{
  return state_ptr->max_code + 1;
}

LZWS_INLINE void lzws_decompressor_initialize_dictionary_wrapper(lzws_decompressor_state_t* state_ptr)
{
  lzws_decompressor_initialize_dictionary(&state_ptr->dictionary);
}

LZWS_INLINE lzws_result_t lzws_decompressor_allocate_dictionary_wrapper(lzws_decompressor_state_t* state_ptr)
{
  size_t total_codes_length = lzws_decompressor_get_total_codes_length(state_ptr);

  lzws_result_t result = lzws_decompressor_allocate_dictionary(&state_ptr->dictionary, total_codes_length, state_ptr->initial_used_code, state_ptr->quiet);
  if (result != 0) {
    return result;
  }

  state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_FIRST_CODE;

  return 0;
}

LZWS_INLINE void lzws_decompressor_write_code_to_dictionary_wrapper(lzws_decompressor_state_t* state_ptr, lzws_code_fast_t code)
{
  lzws_decompressor_write_code_to_dictionary(&state_ptr->dictionary, code);
}
LZWS_INLINE void lzws_decompressor_add_code_to_dictionary_wrapper(lzws_decompressor_state_t* state_ptr, lzws_code_fast_t prefix_code, lzws_code_fast_t current_code, lzws_code_fast_t next_code)
{
  lzws_decompressor_add_code_to_dictionary(&state_ptr->dictionary, prefix_code, current_code, next_code);
}

LZWS_INLINE bool lzws_decompressor_has_symbol_in_dictionary_wrapper(lzws_decompressor_state_t* state_ptr)
{
  return lzws_decompressor_has_symbol_in_dictionary(&state_ptr->dictionary);
}
LZWS_INLINE uint8_t lzws_decompressor_get_symbol_from_dictionary_wrapper(lzws_decompressor_state_t* state_ptr)
{
  return lzws_decompressor_get_symbol_from_dictionary(&state_ptr->dictionary);
}

LZWS_INLINE void lzws_decompressor_free_dictionary_wrapper(lzws_decompressor_state_t* state_ptr)
{
  lzws_decompressor_free_dictionary(&state_ptr->dictionary);
}

#endif // LZWS_DECOMPRESSOR_DICTIONARY_WRAPPER_H
