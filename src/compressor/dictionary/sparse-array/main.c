// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY_MAIN_C

#include "../../common.h"

#include "main.h"

static inline size_t get_codes_length(lzws_compressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  return (lzws_get_power_of_two(max_code_bits) - dictionary_ptr->codes_length_offset) * LZWS_ALPHABET_LENGTH;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  size_t codes_length = get_codes_length(dictionary_ptr, max_code_bits);

  lzws_code_t  undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_code_t* codes               = lzws_allocate_array(
    sizeof(lzws_code_t), codes_length,
    &undefined_next_code, LZWS_UNDEFINED_NEXT_CODE_ZERO, LZWS_UNDEFINED_NEXT_CODE_IDENTICAL_BYTES);
  if (codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary_ptr->next_codes = codes;

  return 0;
}

void lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  size_t codes_length = get_codes_length(dictionary_ptr, max_code_bits);

  lzws_code_t undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_fill_array(
    dictionary_ptr->next_codes,
    sizeof(lzws_code_t), codes_length,
    &undefined_next_code, LZWS_UNDEFINED_NEXT_CODE_IDENTICAL_BYTES);
}
