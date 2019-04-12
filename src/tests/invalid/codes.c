// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../compressor/code.h"
#include "../../compressor/header.h"
#include "../../compressor/remainder.h"
#include "../../macro.h"
#include "../../string.h"
#include "../combinations.h"

#include "codes.h"

typedef struct {
  const lzws_code_t* codes;
  const size_t       codes_length;
} data_t;

// -- any mode --

// First free code should be a char code.
static const lzws_code_t data0_1[] = {LZWS_FIRST_FREE_CODE};

// Last code is greater than next code (equals to first free code).
static const lzws_code_t data1_2[] = {1, LZWS_FIRST_FREE_CODE + 1};

static const data_t datas[] = {
  {data0_1, 1},
  {data1_2, 2}};
static const size_t datas_length = sizeof(datas) / sizeof(data_t);

// -- block mode --

// Code after clear code should be a char code.
static const lzws_code_t data2_3[] = {1, LZWS_CLEAR_CODE, LZWS_FIRST_FREE_CODE_IN_BLOCK_MODE};

// Last code is greater than next code (equals to first free code).
static const lzws_code_t data3_4[] = {1, LZWS_CLEAR_CODE, 1, LZWS_FIRST_FREE_CODE_IN_BLOCK_MODE + 1};

static const data_t block_mode_datas[] = {
  {data2_3, 3},
  {data3_4, 4}};
static const size_t block_mode_datas_length = sizeof(block_mode_datas) / sizeof(data_t);

// -- buffers --

#define SOURCE_LENGTH 8 // 3 bytes for header + 5 bytes for 4 x 9-bit codes.
static uint8_t source[SOURCE_LENGTH];

lzws_result_t process_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, const data_t* data_ptr)
{
  uint8_t* source_ptr    = source;
  size_t   source_length = SOURCE_LENGTH;

  if (
    lzws_compressor_write_magic_header(&source_ptr, &source_length) != 0 ||
    lzws_compressor_write_header(compressor_state_ptr, &source_ptr, &source_length) != 0) {
    return 1;
  }

  for (size_t index = 0; index < data_ptr->codes_length; index++) {
    lzws_code_t code = data_ptr->codes[index];

    if (lzws_compressor_write_code(compressor_state_ptr, code, &source_ptr, &source_length) != 0) {
      return 2;
    }
  }

  if (lzws_compressor_flush_remainder(compressor_state_ptr, &source_ptr, &source_length) != 0) {
    return 3;
  }

  lzws_compressor_clear_state(compressor_state_ptr);

  source_ptr    = source;
  source_length = SOURCE_LENGTH - source_length;

  uint8_t* destination_ptr;
  size_t   destination_length;

  lzws_result_t result = lzws_decompress_string(
    source_ptr, source_length,
    &destination_ptr, &destination_length, 0,
    decompressor_state_ptr->msb, decompressor_state_ptr->unaligned_bit_groups, decompressor_state_ptr->quiet);

  if (result == 0) {
    free(destination_ptr);
    return 4;
  }
  if (result != LZWS_STRING_DECOMPRESSOR_FAILED) {
    return 5;
  }

  return 0;
}

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list LZWS_UNUSED(args))
{
  for (size_t index = 0; index < datas_length; index++) {
    if (process_data(compressor_state_ptr, decompressor_state_ptr, &datas[index]) != 0) {
      return 1;
    }
  }

  if (compressor_state_ptr->block_mode) {
    for (size_t index = 0; index < block_mode_datas_length; index++) {
      if (process_data(compressor_state_ptr, decompressor_state_ptr, &block_mode_datas[index]) != 0) {
        return 2;
      }
    }
  }

  return 0;
}

lzws_result_t lzws_test_invalid_codes()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_data);
}
