// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_DECOMPRESSOR_DICTIONARY_MAIN_C

#include "../../log.h"
#include "../common.h"

#include "main.h"

lzws_result_t lzws_decompressor_allocate_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, size_t total_codes_length, lzws_code_fast_t first_non_char_code, bool quiet)
{
  // We won't store char codes and clear code.
  lzws_code_fast_t codes_offset = first_non_char_code;
  dictionary_ptr->codes_offset  = codes_offset;

  size_t codes_length = total_codes_length - codes_offset;

  // Previous codes don't require default values.
  // Algorithm will access only initialized codes.

  size_t       previous_codes_size = codes_length * sizeof(lzws_code_t);
  lzws_code_t* previous_codes      = malloc(previous_codes_size);

  if (previous_codes == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, previous codes size: %zu", previous_codes_size)
    }

    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  // Last symbol by codes don't require default values.
  // Algorithm will access only initialized symbols.

  size_t   last_symbol_by_codes_size = codes_length;
  uint8_t* last_symbol_by_codes      = malloc(last_symbol_by_codes_size);

  if (last_symbol_by_codes == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, last symbol by codes size: %zu", last_symbol_by_codes_size)
    }

    // Previous codes was allocated, need to free it.
    free(previous_codes);

    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  // Output buffer by codes don't require default values.
  // Algorithm will access only initialized buffer data.

  size_t   output_size   = codes_length;
  uint8_t* output_buffer = malloc(output_size);

  if (output_buffer == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, output size: %zu", output_size)
    }

    // Previous codes and last symbol by codes were allocated, need to free it.
    free(previous_codes);
    free(last_symbol_by_codes);

    return LZWS_DECOMPRESSOR_ALLOCATE_FAILED;
  }

  dictionary_ptr->previous_codes       = previous_codes;
  dictionary_ptr->last_symbol_by_codes = last_symbol_by_codes;
  dictionary_ptr->output_buffer        = output_buffer;

  return 0;
}

static inline uint8_t prepare_output(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code, bool is_prefix)
{
  // First symbol equals last symbol when code is a prefix.
  // Output buffer is reversed.
  // So we need to copy last symbol into the first position.

  size_t output_length;
  if (is_prefix) {
    output_length = 1;
  }
  else {
    output_length = 0;
  }

  lzws_code_fast_t codes_offset = dictionary_ptr->codes_offset;

  lzws_code_t* previous_codes       = dictionary_ptr->previous_codes;
  uint8_t*     last_symbol_by_codes = dictionary_ptr->last_symbol_by_codes;

  uint8_t* output_buffer = dictionary_ptr->output_buffer;

  lzws_code_fast_t code_index;

  // It is not possible to receive clear code during code sequence.
  // So we can compare first code with alphabet length.

  while (code >= LZWS_ALPHABET_LENGTH) {
    code_index = code - codes_offset;

    output_buffer[output_length] = last_symbol_by_codes[code_index];
    output_length++;

    code = previous_codes[code_index];
  }

  uint8_t first_symbol = code;

  output_buffer[output_length] = first_symbol;
  output_length++;

  if (is_prefix) {
    // Last symbol equals to first symbol.
    output_buffer[0] = first_symbol;
  }

  dictionary_ptr->output_length = output_length;

  return first_symbol;
}

void lzws_decompressor_write_code_to_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t code)
{
  prepare_output(dictionary_ptr, code, false);
}

// Example: (ab)(cde).
// Prefix code - ab, current code - cde, next code - abc.
// We can see that last symbol of next code equals to first symbol of current code.

void lzws_decompressor_add_code_to_dictionary(lzws_decompressor_dictionary_t* dictionary_ptr, lzws_code_fast_t prefix_code, lzws_code_fast_t current_code, lzws_code_fast_t next_code)
{
  bool is_prefix = current_code == next_code;

  lzws_code_fast_t code;
  if (is_prefix) {
    code = prefix_code;
  }
  else {
    code = current_code;
  }

  uint8_t first_symbol = prepare_output(dictionary_ptr, code, is_prefix);

  lzws_code_fast_t next_code_index = next_code - dictionary_ptr->codes_offset;

  dictionary_ptr->previous_codes[next_code_index]       = prefix_code;
  dictionary_ptr->last_symbol_by_codes[next_code_index] = first_symbol;
}
