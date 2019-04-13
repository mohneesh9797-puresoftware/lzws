// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

#include "../../log.h"
#include "../../macro.h"
#include "../../string.h"
#include "../codes.h"
#include "../combinations.h"

#include "codes.h"

typedef struct {
  const lzws_code_t* codes;
  const size_t       codes_length;

  const uint8_t* symbols;
  const size_t   symbols_length;
} data_t;

// -- any mode --

static const lzws_code_t data0_1[]    = {254};
static const uint8_t     symbols0_1[] = {254};

static const lzws_code_t data1_2[]    = {250, 4};
static const uint8_t     symbols1_2[] = {250, 4};

static const lzws_code_t data2_3[]    = {255, 3, 150};
static const uint8_t     symbols2_3[] = {255, 3, 150};

static const data_t datas[] = {
  {data0_1, 1, symbols0_1, 1},
  {data1_2, 2, symbols1_2, 2},
  {data2_3, 3, symbols2_3, 3}};
static const size_t datas_length = sizeof(datas) / sizeof(data_t);

// -- block mode --

static const lzws_code_t data3_2[]    = {5, LZWS_CLEAR_CODE};
static const uint8_t     symbols3_1[] = {5};

static const lzws_code_t data4_3[]    = {100, LZWS_CLEAR_CODE, 200};
static const uint8_t     symbols4_2[] = {100, 200};

static const lzws_code_t data5_4[]    = {70, LZWS_CLEAR_CODE, 30, LZWS_CLEAR_CODE};
static const uint8_t     symbols5_2[] = {70, 30};

static const data_t datas_for_block_mode[] = {
  {data3_2, 2, symbols3_1, 1},
  {data4_3, 3, symbols4_2, 2},
  {data5_4, 4, symbols5_2, 2}};
static const size_t datas_for_block_mode_length = sizeof(datas_for_block_mode) / sizeof(data_t);

// -- block mode disabled --

static const lzws_code_t data6_2[]    = {8, LZWS_FIRST_FREE_CODE};
static const uint8_t     symbols6_3[] = {8, 8, 8};

static const lzws_code_t data7_3[]    = {15, 20, LZWS_FIRST_FREE_CODE + 1};
static const uint8_t     symbols7_4[] = {15, 20, 20, 20};

// First free code will be equals to '29'.
static const lzws_code_t data8_3[]    = {2, 9, LZWS_FIRST_FREE_CODE};
static const uint8_t     symbols8_4[] = {2, 9, 2, 9};

static const data_t datas_for_block_mode_disabled[] = {
  {data6_2, 2, symbols6_3, 3},
  {data7_3, 3, symbols7_4, 4},
  {data8_3, 3, symbols8_4, 4}};
static const size_t datas_for_block_mode_disabled_length = sizeof(datas_for_block_mode_disabled) / sizeof(data_t);

// -- common --

#define SOURCE_LENGTH 8 // 3 bytes for header + 5 bytes for 4 x 9-bit codes.
static uint8_t source[SOURCE_LENGTH];

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, const data_t* data_ptr)
{
  uint8_t* source_ptr    = source;
  size_t   source_length = SOURCE_LENGTH;

  if (lzws_test_compressor_write_codes(compressor_state_ptr, data_ptr->codes, data_ptr->codes_length, &source_ptr, &source_length) != 0) {
    LZWS_LOG_ERROR("compressor failed to write codes");
    return 1;
  }

  lzws_compressor_clear_state(compressor_state_ptr);

  source_ptr    = source;
  source_length = SOURCE_LENGTH - source_length;

  uint8_t* destination_ptr;
  size_t   destination_length;

  if (
    lzws_decompress_string(
      source_ptr, source_length,
      &destination_ptr, &destination_length, 0,
      decompressor_state_ptr->msb, decompressor_state_ptr->unaligned_bit_groups, decompressor_state_ptr->quiet) != 0) {
    return 2;
  }

  lzws_result_t result;

  if (destination_length != data_ptr->symbols_length) {
    result = 3;
  }
  else if (strncmp((const char*)destination_ptr, (const char*)data_ptr->symbols, destination_length) != 0) {
    result = 4;
  }
  else {
    result = 0;
  }

  free(destination_ptr);
  return result;
}

lzws_result_t test_datas(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, const data_t* datas, size_t datas_length)
{
  lzws_result_t result;

  for (size_t index = 0; index < datas_length; index++) {
    result = test_data(compressor_state_ptr, decompressor_state_ptr, &datas[index]);
    if (result != 0) {
      return result;
    }
  }

  return 0;
}

lzws_result_t test_all_datas(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list LZWS_UNUSED(args))
{
  if (test_datas(compressor_state_ptr, decompressor_state_ptr, datas, datas_length) != 0) {
    return 1;
  }

  if (compressor_state_ptr->block_mode) {
    // Codes test won't provide alignment bits.
    if (compressor_state_ptr->unaligned_bit_groups) {
      if (test_datas(compressor_state_ptr, decompressor_state_ptr, datas_for_block_mode, datas_for_block_mode_length) != 0) {
        return 2;
      }
    }
  }
  else if (test_datas(compressor_state_ptr, decompressor_state_ptr, datas_for_block_mode_disabled, datas_for_block_mode_disabled_length) != 0) {
    return 3;
  }

  return 0;
}

lzws_result_t lzws_test_basic_codes()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_all_datas);
}
