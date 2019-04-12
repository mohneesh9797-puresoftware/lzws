// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../codes.h"
#include "../../log.h"
#include "../../macro.h"
#include "../../string.h"
#include "../combinations.h"

#include "codes.h"

typedef struct {
  const lzws_code_t* codes;
  const size_t       codes_length;
} data_t;

// -- block mode --

// First free code should be a char code.
static const lzws_code_t data0_1[] = {LZWS_FIRST_FREE_CODE_IN_BLOCK_MODE};

// Last code is greater than next code (equals to first free code).
static const lzws_code_t data1_2[] = {1, LZWS_FIRST_FREE_CODE_IN_BLOCK_MODE + 1};

// Code after clear code should be a char code.
static const lzws_code_t data2_3[] = {1, LZWS_CLEAR_CODE, LZWS_FIRST_FREE_CODE_IN_BLOCK_MODE};

// Last code is greater than next code (equals to first free code).
static const lzws_code_t data3_4[] = {1, LZWS_CLEAR_CODE, 1, LZWS_FIRST_FREE_CODE_IN_BLOCK_MODE + 1};

static const data_t datas_for_block_mode[] = {
  {data0_1, 1},
  {data1_2, 2},
  {data2_3, 3},
  {data3_4, 4}};
static const size_t datas_for_block_mode_length = sizeof(datas_for_block_mode) / sizeof(data_t);

// -- block mode disabled --

// First free code should be a char code.
static const lzws_code_t data4_1[] = {LZWS_FIRST_FREE_CODE};

// Last code is greater than next code (equals to first free code).
static const lzws_code_t data5_2[] = {1, LZWS_FIRST_FREE_CODE + 1};

static const data_t datas_for_block_mode_disabled[] = {
  {data4_1, 1},
  {data5_2, 2}};
static const size_t datas_for_block_mode_disabled_length = sizeof(datas_for_block_mode_disabled) / sizeof(data_t);

// -- buffers --

#define BUFFER_LENGTH 8 // 3 bytes for header + 5 bytes for 4 x 9-bit codes.
static uint8_t buffer[BUFFER_LENGTH];

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, const data_t* data_ptr)
{
  uint8_t* source_ptr    = buffer;
  size_t   source_length = BUFFER_LENGTH;

  if (lzws_test_compressor_write_codes(compressor_state_ptr, data_ptr->codes, data_ptr->codes_length, &source_ptr, &source_length) != 0) {
    LZWS_LOG_ERROR("compressor failed to write codes");
    return 1;
  }

  source_ptr    = buffer;
  source_length = BUFFER_LENGTH - source_length;

  uint8_t* destination_ptr;
  size_t   destination_length;

  lzws_result_t result = lzws_decompress_string(
    source_ptr, source_length,
    &destination_ptr, &destination_length, 0,
    decompressor_state_ptr->msb, decompressor_state_ptr->unaligned_bit_groups, decompressor_state_ptr->quiet);

  if (result == 0) {
    free(destination_ptr);
    LZWS_LOG_ERROR("string decompressor succeeded instead of fail");
    return 4;
  }
  if (result != LZWS_STRING_DECOMPRESSOR_FAILED) {
    LZWS_LOG_ERROR("string decompressor failed with wrong result");
    return 5;
  }

  return 0;
}

lzws_result_t test_datas(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, const data_t* datas, size_t datas_length)
{
  for (size_t index = 0; index < datas_length; index++) {
    if (test_data(compressor_state_ptr, decompressor_state_ptr, &datas[index]) != 0) {
      return 1;
    }
  }

  return 0;
}

lzws_result_t test_all_datas(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list LZWS_UNUSED(args))
{
  if (compressor_state_ptr->block_mode) {
    // Codes test won't provide alignment bits.
    if (compressor_state_ptr->unaligned_bit_groups) {
      if (test_datas(compressor_state_ptr, decompressor_state_ptr, datas_for_block_mode, datas_for_block_mode_length) != 0) {
        return 1;
      }
    }
  }
  else if (test_datas(compressor_state_ptr, decompressor_state_ptr, datas_for_block_mode_disabled, datas_for_block_mode_disabled_length) != 0) {
    return 2;
  }

  return 0;
}

lzws_result_t lzws_test_invalid_codes()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_all_datas);
}
