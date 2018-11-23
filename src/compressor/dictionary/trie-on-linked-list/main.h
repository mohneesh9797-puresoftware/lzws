// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_H_
#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_H_

#include <stdint.h>

#include "../../../common.h"

typedef struct {
  lzws_code_t* first_child_codes;
  lzws_code_t* next_sibling_codes;
  uint8_t*     symbol_by_codes;
} lzws_compressor_dictionary_t;

void          lzws_compressor_dictionary_initialize(lzws_compressor_dictionary_t* dictionary);
lzws_result_t lzws_compressor_dictionary_allocate(lzws_compressor_dictionary_t* dictionary, uint8_t max_code_bits, lzws_code_t initial_code_offset);
lzws_code_t   lzws_compressor_dictionary_get_next_code(lzws_compressor_dictionary_t* dictionary, lzws_code_t initial_code_offset, lzws_code_t current_code, uint8_t symbol);
void          lzws_compressor_dictionary_save_next_code(lzws_compressor_dictionary_t* dictionary, lzws_code_t initial_code_offset, lzws_code_t current_code, lzws_code_t code, uint8_t symbol);
void          lzws_compressor_dictionary_free(lzws_compressor_dictionary_t* dictionary);

#endif // LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_LINKED_LIST_MAIN_H_
