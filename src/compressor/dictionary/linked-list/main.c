// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_C

#include "../../common.h"

#include "main.h"

static inline size_t get_total_codes_length(uint_fast8_t max_code_bits) {
  return lzws_get_power_of_two(max_code_bits);
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  size_t       total_codes_length  = get_total_codes_length(max_code_bits);
  uint_fast8_t codes_length_offset = dictionary_ptr->codes_length_offset;

  lzws_code_t undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;

  lzws_code_t* first_child_codes = lzws_allocate_array(
      sizeof(lzws_code_t), total_codes_length,
      &undefined_next_code, LZWS_UNDEFINED_NEXT_CODE_ZERO, LZWS_UNDEFINED_NEXT_CODE_IDENTICAL_BYTES);
  if (first_child_codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  lzws_code_t* next_sibling_codes = lzws_allocate_array(
      sizeof(lzws_code_t), total_codes_length - codes_length_offset,
      &undefined_next_code, LZWS_UNDEFINED_NEXT_CODE_ZERO, LZWS_UNDEFINED_NEXT_CODE_IDENTICAL_BYTES);
  if (next_sibling_codes == NULL) {
    // "first_child_codes" was allocated, need to free it.
    free(first_child_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  // Last symbol by codes doesn't require default values.
  // Algorithm will access only initialized symbols.
  uint8_t* last_symbol_by_codes = malloc(total_codes_length - codes_length_offset);
  if (last_symbol_by_codes == NULL) {
    // "first_child_codes" and "next_sibling_codes" were allocated, need to free it.
    free(first_child_codes);
    free(next_sibling_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary_ptr->first_child_codes    = first_child_codes;
  dictionary_ptr->next_sibling_codes   = next_sibling_codes;
  dictionary_ptr->last_symbol_by_codes = last_symbol_by_codes;

  return 0;
}

void lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, uint_fast8_t max_code_bits) {
  size_t       total_codes_length  = get_total_codes_length(max_code_bits);
  uint_fast8_t codes_length_offset = dictionary_ptr->codes_length_offset;

  lzws_code_t undefined_next_code = LZWS_UNDEFINED_NEXT_CODE;
  lzws_fill_array(
      dictionary_ptr->first_child_codes,
      sizeof(lzws_code_t), total_codes_length,
      &undefined_next_code, LZWS_UNDEFINED_NEXT_CODE_IDENTICAL_BYTES);
  lzws_fill_array(
      dictionary_ptr->next_sibling_codes,
      sizeof(lzws_code_t), total_codes_length - codes_length_offset,
      &undefined_next_code, LZWS_UNDEFINED_NEXT_CODE_IDENTICAL_BYTES);

  // We can keep last symbol by codes as is.
  // Algorithm will access only initialized symbols.
}

lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol) {
  lzws_code_fast_t first_child_code = dictionary_ptr->first_child_codes[current_code];
  if (first_child_code == LZWS_UNDEFINED_NEXT_CODE) {
    // First child is not found.
    return LZWS_UNDEFINED_NEXT_CODE;
  }

  // We need to find target next symbol in next siblings.

  uint_fast8_t codes_length_offset  = dictionary_ptr->codes_length_offset;
  lzws_code_t* next_sibling_codes   = dictionary_ptr->next_sibling_codes;
  uint8_t*     last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;

  // We know that "first_child_code" is not undefined, so it is better to use do + while for it.
  lzws_code_fast_t next_sibling_code = first_child_code;

  do {
    lzws_code_fast_t last_symbol_by_code_index = next_sibling_code - codes_length_offset;
    if (last_symbol_by_codes[last_symbol_by_code_index] == next_symbol) {
      // We found target next symbol.
      return next_sibling_code;
    }

    lzws_code_fast_t next_sibling_code_index = next_sibling_code - codes_length_offset;
    next_sibling_code                        = next_sibling_codes[next_sibling_code_index];
  } while (next_sibling_code != LZWS_UNDEFINED_NEXT_CODE);

  // Next sibling is not found.
  return LZWS_UNDEFINED_NEXT_CODE;
}

void lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol, lzws_code_fast_t next_code) {
  uint_fast8_t codes_length_offset = dictionary_ptr->codes_length_offset;

  // We need to store next symbol for next code.
  lzws_code_fast_t last_symbol_by_code_index                      = next_code - codes_length_offset;
  dictionary_ptr->last_symbol_by_codes[last_symbol_by_code_index] = next_symbol;

  lzws_code_t*     first_child_codes = dictionary_ptr->first_child_codes;
  lzws_code_fast_t first_child_code  = first_child_codes[current_code];

  if (first_child_code == LZWS_UNDEFINED_NEXT_CODE) {
    // Adding first child.
    first_child_codes[current_code] = next_code;
    return;
  }

  // Adding next sibling.
  lzws_code_t*     next_sibling_codes = dictionary_ptr->next_sibling_codes;
  lzws_code_fast_t next_code_index    = next_code - codes_length_offset;

  first_child_codes[current_code]     = next_code;
  next_sibling_codes[next_code_index] = first_child_code;
}
