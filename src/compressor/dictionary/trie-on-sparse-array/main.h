// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H)
#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H

#include <stdlib.h>

#include "type.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t initial_used_code) {
  // We won't store clear code.
  dictionary->codes_length_offset = initial_used_code + 1 - LZWS_ALPHABET_LENGTH;

  dictionary->codes = NULL;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits);
void          lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits);

LZWS_INLINE size_t lzws_compressor_get_code_index(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t current_code, uint_fast8_t next_symbol) {
  // We need to remove code offset from current code when it is not a char code.
  if (current_code > LZWS_ALPHABET_LENGTH) {
    current_code -= dictionary->codes_length_offset;
  }

  return current_code * LZWS_ALPHABET_LENGTH + next_symbol;
}

LZWS_INLINE lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t current_code, uint_fast8_t next_symbol) {
  size_t code_index = lzws_compressor_get_code_index(dictionary, current_code, next_symbol);
  return dictionary->codes[code_index];
}

LZWS_INLINE void lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t current_code, uint_fast8_t next_symbol, lzws_code_fast_t next_code) {
  size_t code_index             = lzws_compressor_get_code_index(dictionary, current_code, next_symbol);
  dictionary->codes[code_index] = next_code;
}

LZWS_INLINE void lzws_compressor_free_dictionary(lzws_compressor_dictionary_t* dictionary) {
  if (dictionary->codes != NULL) {
    free(dictionary->codes);
  }
}

#endif // LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H
