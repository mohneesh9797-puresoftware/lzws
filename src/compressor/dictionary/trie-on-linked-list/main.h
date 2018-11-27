// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_H)
#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_H

#include <stdlib.h>

#include "../../../common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

typedef struct {
  uint8_t initial_code_offset;

  lzws_code_t* first_child_codes;
  lzws_code_t* next_sibling_codes;
  uint8_t*     symbol_by_codes;
} lzws_compressor_dictionary_t;

LZWS_INLINE void lzws_compressor_initialize_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_t initial_used_code) {
  dictionary->initial_code_offset = initial_used_code + 1;

  dictionary->first_child_codes  = NULL;
  dictionary->next_sibling_codes = NULL;
  dictionary->symbol_by_codes    = NULL;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary, uint8_t max_code_bits);
lzws_code_t   lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_t current_code, uint8_t symbol);
void          lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_t current_code, uint8_t symbol, lzws_code_t code);

LZWS_INLINE void lzws_compressor_free_dictionary(lzws_compressor_dictionary_t* dictionary) {
  if (dictionary->first_child_codes != NULL) {
    free(dictionary->first_child_codes);
  }

  if (dictionary->next_sibling_codes != NULL) {
    free(dictionary->next_sibling_codes);
  }

  if (dictionary->symbol_by_codes != NULL) {
    free(dictionary->symbol_by_codes);
  }
}

#endif // LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_H
