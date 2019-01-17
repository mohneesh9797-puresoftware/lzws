// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST_MAIN_C

#include "../../common.h"

#include "main.h"

static inline size_t get_first_child_codes_length(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  // We are operating first child codes: current code -> next code.
  // Maximum next code is max code, maximum current code will be max code - 1.
  // So we don't need to store max code.
  return total_codes_length - dictionary_ptr->first_child_codes_offset - 1;
}

static inline size_t get_next_sibling_codes_length(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  return total_codes_length - dictionary_ptr->next_sibling_codes_offset;
}

static inline lzws_code_fast_t get_first_child_code_index(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code) {
  // We need to remove offset from code when it is not a char code.
  if (code > LZWS_ALPHABET_LENGTH) {
    code -= dictionary_ptr->first_child_codes_offset;
  }

  return code;
}

static inline lzws_code_fast_t get_next_sibling_code_index(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code) {
  return code - dictionary_ptr->next_sibling_codes_offset;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  lzws_code_t undefined_next_code = LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE;

  size_t first_child_codes_length = get_first_child_codes_length(dictionary_ptr, total_codes_length);

  lzws_code_t* first_child_codes = lzws_allocate_array(
    sizeof(lzws_code_t), first_child_codes_length, &undefined_next_code,
    LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_IS_ZERO, LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_HAS_IDENTICAL_BYTES);
  if (first_child_codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  size_t next_sibling_codes_length = get_next_sibling_codes_length(dictionary_ptr, total_codes_length);

  lzws_code_t* next_sibling_codes = lzws_allocate_array(
    sizeof(lzws_code_t), next_sibling_codes_length, &undefined_next_code,
    LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_IS_ZERO, LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_HAS_IDENTICAL_BYTES);
  if (next_sibling_codes == NULL) {
    // First child codes was allocated, need to free it.
    free(first_child_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  // Last symbol by codes don't require default values.
  // Algorithm will access only initialized symbols.
  uint8_t* last_symbol_by_codes = malloc(next_sibling_codes_length);
  if (last_symbol_by_codes == NULL) {
    // First child codes and next sibling codes were allocated, need to free it.
    free(first_child_codes);
    free(next_sibling_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary_ptr->first_child_codes    = first_child_codes;
  dictionary_ptr->next_sibling_codes   = next_sibling_codes;
  dictionary_ptr->last_symbol_by_codes = last_symbol_by_codes;

  return 0;
}

void lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  lzws_code_t undefined_next_code = LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE;

  size_t first_child_codes_length = get_first_child_codes_length(dictionary_ptr, total_codes_length);
  lzws_fill_array(
    dictionary_ptr->first_child_codes,
    sizeof(lzws_code_t), first_child_codes_length, &undefined_next_code,
    LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_HAS_IDENTICAL_BYTES);

  size_t next_sibling_codes_length = get_next_sibling_codes_length(dictionary_ptr, total_codes_length);
  lzws_fill_array(
    dictionary_ptr->next_sibling_codes,
    sizeof(lzws_code_t), next_sibling_codes_length, &undefined_next_code,
    LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_HAS_IDENTICAL_BYTES);

  // We can keep last symbol by codes as is.
  // Algorithm will access only initialized symbols.
}

lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol) {
  lzws_code_fast_t current_code_index = get_first_child_code_index(dictionary_ptr, current_code);

  lzws_code_fast_t first_child_code = dictionary_ptr->first_child_codes[current_code_index];
  if (first_child_code == LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE) {
    // First child is not found.
    return LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE;
  }

  // We need to find target next symbol in next siblings.

  lzws_code_t* next_sibling_codes   = dictionary_ptr->next_sibling_codes;
  uint8_t*     last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;

  // We know that first child code is not undefined, so it is better to use do + while for it.
  lzws_code_fast_t next_sibling_code = first_child_code;

  do {
    lzws_code_fast_t next_sibling_code_index = get_next_sibling_code_index(dictionary_ptr, next_sibling_code);
    if (last_symbol_by_codes[next_sibling_code_index] == next_symbol) {
      // We found target next symbol.
      return next_sibling_code;
    }

    next_sibling_code = next_sibling_codes[next_sibling_code_index];
  } while (next_sibling_code != LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE);

  // Next sibling is not found.
  return LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE;
}

void lzws_compressor_save_next_code_to_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol, lzws_code_fast_t next_code) {
  lzws_code_fast_t next_code_index = get_next_sibling_code_index(dictionary_ptr, next_code);

  // We need to store next symbol for next code.
  dictionary_ptr->last_symbol_by_codes[next_code_index] = next_symbol;

  lzws_code_fast_t current_code_index = get_first_child_code_index(dictionary_ptr, current_code);

  lzws_code_t*     first_child_codes = dictionary_ptr->first_child_codes;
  lzws_code_fast_t first_child_code  = first_child_codes[current_code_index];
  if (first_child_code == LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE) {
    // Adding first child.
    first_child_codes[current_code_index] = next_code;
    return;
  }

  // Adding next sibling.
  first_child_codes[current_code_index]               = next_code;
  dictionary_ptr->next_sibling_codes[next_code_index] = first_child_code;
}