// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../decompressor/header.h"
#include "../../decompressor/common.h"
#include "../../macro.h"
#include "../combinations.h"

#include "header.h"

#define MAGIC_HEADER_VALUE_SIZE 2
static const uint8_t magic_header_values[][MAGIC_HEADER_VALUE_SIZE] = {
  {LZWS_FIRST_MAGIC_HEADER_BYTE + 1, LZWS_SECOND_MAGIC_HEADER_BYTE}, // First byte is invalid.
  {LZWS_FIRST_MAGIC_HEADER_BYTE, LZWS_SECOND_MAGIC_HEADER_BYTE + 1}  // Second byte is invalid.
};
#define MAGIC_HEADER_VALUES_LENGTH sizeof(magic_header_values) / (MAGIC_HEADER_VALUE_SIZE * sizeof(uint8_t))

static const uint8_t max_code_bit_length_header = (LZWS_LOWEST_MAX_CODE_BIT_LENGTH - 1) | LZWS_BLOCK_MODE;
#define MAX_CODE_BIT_LENGTH_HEADER_SIZE sizeof(max_code_bit_length_header) * sizeof(uint8_t)

static inline lzws_result_t test_invalid_header(lzws_decompressor_state_t* state_ptr, va_list LZWS_UNUSED(args))
{
  for (size_t index = 0; index < MAGIC_HEADER_VALUES_LENGTH; index++) {
    uint8_t* magic_header_value      = (uint8_t*)magic_header_values[index];
    size_t   magic_header_value_size = MAGIC_HEADER_VALUE_SIZE;

    if (lzws_decompressor_read_magic_header(state_ptr, &magic_header_value, &magic_header_value_size) != LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER) {
      return 2;
    }
  }

  uint8_t* max_code_bit_length_header_ptr  = (uint8_t*)&max_code_bit_length_header;
  size_t   max_code_bit_length_header_size = MAX_CODE_BIT_LENGTH_HEADER_SIZE;

  if (lzws_decompressor_read_header(state_ptr, &max_code_bit_length_header_ptr, &max_code_bit_length_header_size) != LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH) {
    return 3;
  }

  return 0;
}

lzws_result_t lzws_test_invalid_header()
{
  return lzws_test_decompressor_combinations(test_invalid_header);
}
