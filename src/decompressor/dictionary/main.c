// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_C

#include "../common.h"

#include "main.h"

static inline size_t get_codes_length(lzws_decompressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  return lzws_get_power_of_two(max_code_bits) - dictionary_ptr->codes_length_offset;
}

lzws_result_t lzws_decompressor_allocate_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  lzws_code_t undefined_previous_code = lzws_get_undefined_previous_code(max_code_bits);
  size_t      codes_length            = get_codes_length(dictionary_ptr, max_code_bits);

  lzws_code_t* previous_codes = lzws_allocate_array(
      sizeof(lzws_code_t), codes_length,
      &undefined_previous_code, LZWS_UNDEFINED_PREVIOUS_CODE_ZERO, LZWS_UNDEFINED_PREVIOUS_CODE_IDENTICAL_BYTES);
  if (previous_codes == NULL) {
    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  // Last symbol by codes doesn't require default values.
  // Algorithm will access only initialized symbols.
  uint8_t* last_symbol_by_codes = malloc(codes_length);
  if (last_symbol_by_codes == NULL) {
    // "previous_codes" was allocated, need to free it.
    free(previous_codes);

    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  // Output buffer by codes doesn't require default values.
  // Algorithm will access only initialized buffer bytes.
  uint8_t* output_buffer = malloc(codes_length);
  if (output_buffer == NULL) {
    // "previous_codes" and "last_symbol_by_codes" were allocated, need to free it.
    free(previous_codes);
    free(last_symbol_by_codes);

    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary_ptr->previous_codes       = previous_codes;
  dictionary_ptr->last_symbol_by_codes = last_symbol_by_codes;
  dictionary_ptr->output_buffer        = output_buffer;

  return 0;
}

void lzws_decompressor_clear_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  lzws_code_t undefined_previous_code = lzws_get_undefined_previous_code(max_code_bits);
  size_t      codes_length            = get_codes_length(dictionary_ptr, max_code_bits);

  lzws_fill_array(
      dictionary_ptr->previous_codes,
      sizeof(lzws_code_t), codes_length,
      &undefined_previous_code, LZWS_UNDEFINED_PREVIOUS_CODE_IDENTICAL_BYTES);

  // We can keep last symbol by codes and output buffer as is.
  // Algorithm will access only initialized symbols and buffer bytes.
}
