// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_DICTIONARY_MAIN_C

#include "../common.h"

#include "main.h"

static inline size_t get_codes_length(lzws_decompressor_dictionary_t* dictionary_ptr, size_t total_codes_length) {
  return total_codes_length - dictionary_ptr->codes_length_offset;
}

lzws_result_t lzws_decompressor_allocate_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t initial_used_code, size_t total_codes_length) {
  // We won't store char codes and clear code.
  dictionary_ptr->codes_length_offset = initial_used_code + 1;

  size_t codes_length = get_codes_length(dictionary_ptr, total_codes_length);

  // Previous codes doesn't require default values.
  // Algorithm will access only initialized codes.
  lzws_code_t* previous_codes = malloc(codes_length * sizeof(lzws_code_t));
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

  // It is possible to keep last_output_index" uninitialized.

  return 0;
}

// void lzws_decompressor_prepare_code_for_writing_in_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code, bool copy_first_symbol_to_last) {
//   lzws_code_fast_t last_output_index;
//   if (copy_first_symbol_to_last) {
//     last_output_index = 1;
//   } else {
//     last_output_index = 0;
//   }
//
//   lzws_code_fast_t codes_length_offset = dictionary_ptr->codes_length_offset;
//
//   lzws_code_t* previous_codes       = dictionary_ptr->previous_codes;
//   uint8_t*     last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;
//
//   uint8_t* output_buffer = dictionary_ptr->output_buffer;
//
//   uint8_t last_symbol;
//
//   while (true) {
//     lzws_code_fast_t last_symbol_by_code_index = code - codes_length_offset;
//     last_symbol                                = last_symbol_by_codes[last_symbol_by_code_index];
//
//     output_buffer[last_output_index] = last_symbol;
//     last_output_index++;
//
//     lzws_code_fast_t previous_code_index = code - codes_length_offset;
//     lzws_code_fast_t previous_code_value = previous_codes[previous_code_index];
//     if (previous_code_value == LZWS_DECOMPRESSOR_DICTIONARY_UNDEFINED_PREVIOUS_CODE) {
//       break;
//     }
//
//     code = previous_code_value - LZWS_DECOMPRESSOR_DICTIONARY_PREVIOUS_CODE_OFFSET;
//   }
//
//   if (copy_first_symbol_to_last) {
//     output_buffer[0] = last_symbol;
//   }
//
//   dictionary_ptr->last_output_index = last_output_index;
// }
