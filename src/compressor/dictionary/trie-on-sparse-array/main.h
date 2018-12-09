// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H)
#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H

#include "../../../macro.h"
#include "../../../utils.h"
#include "../../common.h"

#include "type.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t LZWS_UNUSED(initial_used_code)) {
  dictionary->codes = NULL;
}

LZWS_INLINE lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits) {
  size_t total_codes = lzws_get_power_of_two(max_code_bits);

  lzws_code_t  undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_code_t* codes               = lzws_allocate_array(sizeof(undefined_next_code), total_codes * LZWS_ALPHABET_LENGTH, &undefined_next_code, true, LZWS_IS_UNDEFINED_NEXT_CODE_ZERO);
  if (codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary->codes = codes;

  return 0;
}

LZWS_INLINE void lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits) {
  size_t total_codes = lzws_get_power_of_two(max_code_bits);

  lzws_code_t undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_fill_array(dictionary->codes, sizeof(undefined_next_code), total_codes * LZWS_ALPHABET_LENGTH, &undefined_next_code, LZWS_IS_UNDEFINED_NEXT_CODE_ZERO);
}

LZWS_INLINE lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t current_code, uint_fast8_t symbol) {
  size_t code_index = current_code * LZWS_ALPHABET_LENGTH + symbol;
  return dictionary->codes[code_index];
}

LZWS_INLINE void lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary, lzws_code_fast_t current_code, uint_fast8_t symbol, lzws_code_fast_t code) {
  size_t code_index             = current_code * LZWS_ALPHABET_LENGTH + symbol;
  dictionary->codes[code_index] = code;
}

LZWS_INLINE void lzws_compressor_free_dictionary(lzws_compressor_dictionary_t* dictionary) {
  if (dictionary->codes != NULL) {
    free(dictionary->codes);
  }
}

#endif // LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H
