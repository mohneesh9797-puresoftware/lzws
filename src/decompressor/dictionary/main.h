// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_DICTIONARY_MAIN_H)
#define LZWS_DECOMPRESSOR_DICTIONARY_MAIN_H

#include "type.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_DICTIONARY_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_decompressor_initialize_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr) {
  dictionary_ptr->previous_codes       = NULL;
  dictionary_ptr->last_symbol_by_codes = NULL;
  dictionary_ptr->output_buffer        = NULL;
}

lzws_result_t lzws_decompressor_allocate_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t initial_used_code, uint_fast8_t max_code_bits);
void          lzws_decompressor_clear_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits);

LZWS_INLINE void lzws_decompressor_free_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr) {
  lzws_code_t* previous_codes = dictionary_ptr->previous_codes;
  if (previous_codes != NULL) {
    free(previous_codes);
  }

  uint8_t* last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;
  if (last_symbol_by_codes != NULL) {
    free(last_symbol_by_codes);
  }

  uint8_t* output_buffer = dictionary_ptr->output_buffer;
  if (output_buffer != NULL) {
    free(output_buffer);
  }
}

#endif // LZWS_DECOMPRESSOR_DICTIONARY_MAIN_H
