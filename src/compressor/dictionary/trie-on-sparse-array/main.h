// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H)
#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H

#include "../../../common.h"
#include "../../../constants.h"
#include "../../../macro.h"
#include "../../../utils.h"
#include "../../common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

typedef struct {
  lzws_code_t* codes;
} lzws_compressor_dictionary_t;

LZWS_INLINE void lzws_compressor_initialize_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_t LZWS_UNUSED(initial_used_code)) {
  dictionary->codes = NULL;
}

LZWS_INLINE lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary, uint8_t max_code_bits) {
  size_t total_codes = LZWS_POWERS_OF_TWO[max_code_bits];

  lzws_code_t* codes = lzws_allocate_array(sizeof(lzws_code_t), total_codes * 256, true, LZWS_UNDEFINED_NEXT_CODE);
  if (codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary->codes = codes;

  return 0;
}

LZWS_INLINE lzws_code_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_t current_code, uint8_t symbol) {
  size_t code_index = current_code * 256 + symbol;
  return dictionary->codes[code_index];
}

LZWS_INLINE void lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_t current_code, uint8_t symbol, lzws_code_t code) {
  size_t code_index             = current_code * 256 + symbol;
  dictionary->codes[code_index] = code;
}

LZWS_INLINE void lzws_compressor_free_dictionary(lzws_compressor_dictionary_t* dictionary) {
  if (dictionary->codes != NULL) {
    free(dictionary->codes);
  }
}

#endif // LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H
