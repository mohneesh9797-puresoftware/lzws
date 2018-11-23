// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../../constants.h"
#include "../../../utils.h"
#include "../../common.h"

#include "main.h"

void lzws_compressor_dictionary_initialize(lzws_compressor_dictionary_t* dictionary, lzws_code_t initial_used_code) {
  dictionary->initial_code_offset = initial_used_code + 1;

  dictionary->first_child_codes  = NULL;
  dictionary->next_sibling_codes = NULL;
  dictionary->symbol_by_codes    = NULL;
}

lzws_result_t lzws_compressor_dictionary_allocate(lzws_compressor_dictionary_t* dictionary, uint8_t max_code_bits) {
  size_t total_codes = LZWS_POWERS_OF_TWO[max_code_bits];

  lzws_code_t* first_child_codes = lzws_allocate_array(sizeof(lzws_code_t), total_codes, true, LZWS_UNDEFINED_NEXT_CODE);
  if (first_child_codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  uint8_t initial_code_offset = dictionary->initial_code_offset;

  lzws_code_t* next_sibling_codes = lzws_allocate_array(sizeof(lzws_code_t), total_codes - initial_code_offset, true, LZWS_UNDEFINED_NEXT_CODE);
  if (next_sibling_codes == NULL) {
    // "first_child_codes" was allocated, need to free it.
    free(first_child_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  uint8_t* symbol_by_codes = lzws_allocate_array(sizeof(uint8_t), total_codes - initial_code_offset, false, 0);
  if (symbol_by_codes == NULL) {
    // "first_child_codes" and "next_sibling_codes" were allocated, need to free it.
    free(first_child_codes);
    free(next_sibling_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary->first_child_codes  = first_child_codes;
  dictionary->next_sibling_codes = next_sibling_codes;
  dictionary->symbol_by_codes    = symbol_by_codes;

  return 0;
}

lzws_code_t lzws_compressor_dictionary_get_next_code(lzws_compressor_dictionary_t* dictionary, lzws_code_t current_code, uint8_t symbol) {
  lzws_code_t first_child_code = dictionary->first_child_codes[current_code];
  if (first_child_code == LZWS_UNDEFINED_NEXT_CODE) {
    // First child is not found.
    return LZWS_UNDEFINED_NEXT_CODE;
  }

  // We need to find target symbol in next siblings.

  uint8_t      initial_code_offset = dictionary->initial_code_offset;
  lzws_code_t* next_sibling_codes  = dictionary->next_sibling_codes;
  uint8_t*     symbol_by_codes     = dictionary->symbol_by_codes;

  // We know that "first_child_code" is not undefined, so it is better to use do + while for it.
  lzws_code_t next_sibling_code = first_child_code;

  do {
    lzws_code_t symbol_by_code_index = next_sibling_code - initial_code_offset;
    if (symbol_by_codes[symbol_by_code_index] == symbol) {
      // We found target symbol.
      return next_sibling_code;
    }

    lzws_code_t next_sibling_code_index = next_sibling_code - initial_code_offset;
    next_sibling_code                   = next_sibling_codes[next_sibling_code_index];
  } while (next_sibling_code != LZWS_UNDEFINED_NEXT_CODE);

  // Next sibling is not found.
  return LZWS_UNDEFINED_NEXT_CODE;
}

void lzws_compressor_dictionary_save_next_code(lzws_compressor_dictionary_t* dictionary, lzws_code_t current_code, lzws_code_t code, uint8_t symbol) {
  // We need to store symbol for this code.
  dictionary->symbol_by_codes[code] = symbol;

  lzws_code_t* first_child_codes = dictionary->first_child_codes;
  lzws_code_t  first_child_code  = first_child_codes[current_code];

  if (first_child_code == LZWS_UNDEFINED_NEXT_CODE) {
    // Adding first child.
    first_child_codes[current_code] = code;
    return;
  }

  // Adding next sibling.
  lzws_code_t* next_sibling_codes = dictionary->next_sibling_codes;
  lzws_code_t  next_code_index    = code - dictionary->initial_code_offset;

  first_child_codes[current_code]     = code;
  next_sibling_codes[next_code_index] = first_child_code;
}

void lzws_compressor_dictionary_free(lzws_compressor_dictionary_t* dictionary) {
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
