// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_C

#include "../../../utils.h"
#include "../../common.h"

#include "main.h"

static inline size_t get_total_codes_length(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits) {
  return (lzws_get_power_of_two(max_code_bits) - dictionary->codes_length_offset) * LZWS_ALPHABET_LENGTH;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits) {
  size_t total_codes_length = get_total_codes_length(dictionary, max_code_bits);

  lzws_code_t  undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_code_t* codes               = lzws_allocate_array(sizeof(lzws_code_t), total_codes_length, &undefined_next_code, LZWS_IS_UNDEFINED_NEXT_CODE_ZERO);
  if (codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary->codes = codes;

  return 0;
}

void lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary, uint_fast8_t max_code_bits) {
  size_t total_codes_length = get_total_codes_length(dictionary, max_code_bits);

  lzws_code_t undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_fill_array(dictionary->codes, sizeof(lzws_code_t), total_codes_length, &undefined_next_code, LZWS_IS_UNDEFINED_NEXT_CODE_ZERO);
}
