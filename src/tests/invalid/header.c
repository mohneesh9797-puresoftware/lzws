// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../decompressor/header.h"
#include "../../decompressor/common.h"

#include "header.h"

#define MAGIC_HEADER_SIZE 2
#define MAGIC_HEADER_LENGTH sizeof(magic_headers) / (sizeof(uint8_t) * MAGIC_HEADER_SIZE)
static const uint8_t magic_headers[][MAGIC_HEADER_SIZE] = {
  {LZWS_FIRST_MAGIC_HEADER_BYTE + 1, LZWS_SECOND_MAGIC_HEADER_BYTE},
  {LZWS_FIRST_MAGIC_HEADER_BYTE, LZWS_SECOND_MAGIC_HEADER_BYTE + 1}};

#define MAX_CODE_BIT_LENGTH_HEADER_SIZE sizeof(max_code_bit_length_header)
static const uint8_t max_code_bit_length_header = (LZWS_LOWEST_MAX_CODE_BIT_LENGTH - 1) | LZWS_BLOCK_MODE;

static inline lzws_result_t test_data(lzws_decompressor_state_t* state_ptr)
{
  for (size_t index = 0; index < MAGIC_HEADER_LENGTH; index++) {
    uint8_t* magic_header      = (uint8_t*)magic_headers[index];
    size_t   magic_header_size = MAGIC_HEADER_SIZE;

    if (lzws_decompressor_read_magic_header(state_ptr, &magic_header, &magic_header_size) != LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER) {
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
  lzws_decompressor_state_t* state_ptr;

  if (lzws_decompressor_get_initial_state(&state_ptr, false, false, false) != 0) {
    return 1;
  }

  lzws_result_t result = test_data(state_ptr);
  lzws_decompressor_free_state(state_ptr);

  return result;
}
