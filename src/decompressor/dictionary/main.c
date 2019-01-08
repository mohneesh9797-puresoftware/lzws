// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_DICTIONARY_MAIN_C

#include "../common.h"

#include "main.h"

lzws_result_t lzws_decompressor_allocate_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t initial_used_code, size_t total_codes_length) {
  // We won't store char codes and clear code.
  lzws_code_fast_t codes_length_offset = initial_used_code + 1;
  dictionary_ptr->codes_length_offset  = codes_length_offset;

  size_t codes_length = total_codes_length - codes_length_offset;

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

  // It is possible to keep "output_length" uninitialized.

  return 0;
}

void lzws_decompressor_prepare_code_for_writing_in_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code, bool is_prefix) {
  // First symbol equals last symbol when code is a prefix.
  // Output buffer is reversed.
  // So we need to copy last symbol into the first position.

  size_t output_length;
  if (is_prefix) {
    output_length = 1;
  } else {
    output_length = 0;
  }

  lzws_code_fast_t codes_length_offset = dictionary_ptr->codes_length_offset;

  lzws_code_t* previous_codes       = dictionary_ptr->previous_codes;
  uint8_t*     last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;

  uint8_t* output_buffer = dictionary_ptr->output_buffer;

  while (code >= LZWS_ALPHABET_LENGTH) {
    output_buffer[output_length] = last_symbol_by_codes[code - codes_length_offset];
    output_length++;

    code = previous_codes[code - codes_length_offset];
  }

  output_buffer[output_length] = code;
  output_length++;

  if (is_prefix) {
    output_buffer[0] = code;
  }

  dictionary_ptr->output_length = output_length;
}
