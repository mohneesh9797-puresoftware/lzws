// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_H)
#define LZWS_DECOMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_H

#include "type.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_decompressor_initialize_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t initial_used_code) {
  // We won't store char codes and clear code.
  dictionary_ptr->codes_length_offset = initial_used_code + 1;

  dictionary_ptr->previous_codes       = NULL;
  dictionary_ptr->last_symbol_by_codes = NULL;
  dictionary_ptr->output_buffer        = NULL;
}

lzws_result_t lzws_decompressor_allocate_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits);

LZWS_INLINE void lzws_decompressor_free_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr) {
  if (dictionary_ptr->previous_codes != NULL) {
    free(dictionary_ptr->previous_codes);
  }

  if (dictionary_ptr->last_symbol_by_codes != NULL) {
    free(dictionary_ptr->last_symbol_by_codes);
  }

  if (dictionary_ptr->output_buffer != NULL) {
    free(dictionary_ptr->output_buffer);
  }
}

#endif // LZWS_DECOMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_H
