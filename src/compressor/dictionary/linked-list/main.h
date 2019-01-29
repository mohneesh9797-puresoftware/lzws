// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_H)
#define LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_H

#include <stdlib.h>

#include "common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t initial_used_code)
{
  dictionary_ptr->first_child_codes    = NULL;
  dictionary_ptr->next_sibling_codes   = NULL;
  dictionary_ptr->last_symbol_by_codes = NULL;

  lzws_code_fast_t first_non_char_code = initial_used_code + 1;
  dictionary_ptr->first_non_char_code  = first_non_char_code;

  // We won't store clear code.
  dictionary_ptr->first_child_codes_offset = first_non_char_code - LZWS_ALPHABET_LENGTH;

  // We won't store char codes and clear code.
  dictionary_ptr->next_sibling_codes_offset = first_non_char_code;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length, bool quiet);
void          lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length);

lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol);
void             lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol, lzws_code_fast_t next_code);

LZWS_INLINE void lzws_compressor_free_dictionary(lzws_compressor_dictionary_t* dictionary_ptr)
{
  lzws_code_t* first_child_codes = dictionary_ptr->first_child_codes;
  if (first_child_codes != NULL) {
    free(first_child_codes);
  }

  lzws_code_t* next_sibling_codes = dictionary_ptr->next_sibling_codes;
  if (next_sibling_codes != NULL) {
    free(next_sibling_codes);
  }

  uint8_t* last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;
  if (last_symbol_by_codes != NULL) {
    free(last_symbol_by_codes);
  }
}

#endif // LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_H
