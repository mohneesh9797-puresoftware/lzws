// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY_MAIN_C

#include "../../common.h"

#include "main.h"

static inline size_t get_next_codes_length(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  return (total_codes_length - dictionary_ptr->next_codes_offset) * LZWS_ALPHABET_LENGTH;
}

static inline size_t get_used_indexes_length(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  return total_codes_length - dictionary_ptr->used_indexes_offset;
}

static inline lzws_compressor_dictionary_used_index_t get_next_code_index(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol) {
  // We need to remove offset from code when it is not a char code.
  if (current_code > LZWS_ALPHABET_LENGTH) {
    current_code -= dictionary_ptr->next_codes_offset;
  }

  return current_code * LZWS_ALPHABET_LENGTH + next_symbol;
}

static inline lzws_code_fast_t get_index_of_used_index(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code) {
  return code - dictionary_ptr->used_indexes_offset;
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length, bool block_mode) {
  lzws_code_t undefined_next_code = LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE;

  size_t next_codes_length = get_next_codes_length(dictionary_ptr, total_codes_length);

  lzws_code_t* next_codes = lzws_allocate_array(
    sizeof(lzws_code_t), next_codes_length, &undefined_next_code,
    LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_IS_ZERO, LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE_HAS_IDENTICAL_BYTES);
  if (next_codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  if (block_mode) {
    size_t used_indexes_length = get_used_indexes_length(dictionary_ptr, total_codes_length);

    // Used indexes don't require default values.
    // Algorithm will access only initialized indexes.
    lzws_compressor_dictionary_used_index_t* used_indexes = malloc(used_indexes_length * sizeof(lzws_compressor_dictionary_used_index_t));
    if (used_indexes == NULL) {
      // Next codes was allocated, need to free it.
      free(next_codes);

      return LZWS_COMPRESSOR_ALLOCATE_FAILED;
    }

    dictionary_ptr->used_indexes = used_indexes;
  }

  dictionary_ptr->next_codes = next_codes;

  return 0;
}

void lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  lzws_code_t*                             next_codes   = dictionary_ptr->next_codes;
  lzws_compressor_dictionary_used_index_t* used_indexes = dictionary_ptr->used_indexes;

  size_t used_indexes_length = get_used_indexes_length(dictionary_ptr, total_codes_length);

  for (lzws_code_fast_t index_of_used_index = 0; index_of_used_index < used_indexes_length; index_of_used_index++) {
    lzws_compressor_dictionary_used_index_t used_index = used_indexes[index_of_used_index];
    next_codes[used_index]                             = LZWS_COMPRESSOR_UNDEFINED_NEXT_CODE;
  }

  // We can keep used indexes as is.
  // Algorithm will access only initialized indexes.
}

lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol) {
  lzws_compressor_dictionary_used_index_t code_index = get_next_code_index(dictionary_ptr, current_code, next_symbol);
  return dictionary_ptr->next_codes[code_index];
}

void lzws_compressor_save_next_code_to_dictionary(
  lzws_compressor_dictionary_t* dictionary_ptr, bool block_mode,
  lzws_code_fast_t current_code, uint_fast8_t next_symbol, lzws_code_fast_t next_code) {
  //

  lzws_compressor_dictionary_used_index_t code_index = get_next_code_index(dictionary_ptr, current_code, next_symbol);

  if (block_mode) {
    lzws_code_fast_t index_of_used_index              = get_index_of_used_index(dictionary_ptr, next_code);
    dictionary_ptr->used_indexes[index_of_used_index] = code_index;
  }

  dictionary_ptr->next_codes[code_index] = next_code;
}