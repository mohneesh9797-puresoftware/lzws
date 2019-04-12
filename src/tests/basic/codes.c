// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

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

// -- no block mode --

static const lzws_code_t data3_2[]    = {5, LZWS_FIRST_FREE_CODE};
static const uint8_t     symbols3_3[] = {5, 5, 5};

static const lzws_code_t data4_3[]    = {7, 3, LZWS_FIRST_FREE_CODE + 1};
static const uint8_t     symbols4_4[] = {7, 3, 3, 3};

// First free code will be equals to '73'.
static const lzws_code_t data5_3[]    = {7, 3, LZWS_FIRST_FREE_CODE};
static const uint8_t     symbols5_4[] = {7, 3, 7, 3};

static const data_t no_block_mode_datas[] = {
  {data3_2, 2, symbols3_3, 3},
  {data4_3, 3, symbols4_4, 4},
  {data5_3, 3, symbols5_4, 4}};
static const size_t no_block_mode_datas_length = sizeof(no_block_mode_datas) / sizeof(data_t);

// -- block mode --

static const lzws_code_t data6_2[]    = {5, LZWS_CLEAR_CODE};
static const uint8_t     symbols6_1[] = {6};

static const data_t block_mode_datas[] = {
  {data6_2, 2, symbols6_1, 1}};
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

  if (
    lzws_decompress_string(
      source_ptr, source_length,
      &destination_ptr, &destination_length, 0,
      decompressor_state_ptr->msb, decompressor_state_ptr->unaligned_bit_groups, decompressor_state_ptr->quiet) != 0) {
    return 4;
  }

  lzws_result_t result;

  if (destination_length != data_ptr->symbols_length) {
    result = 5;
  }
  else if (strncmp((const char*)destination_ptr, (const char*)data_ptr->symbols, destination_length) != 0) {
    result = 6;
  }
  else {
    result = 0;
  }

  free(destination_ptr);
  return result;
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
  else {
    for (size_t index = 0; index < no_block_mode_datas_length; index++) {
      if (process_data(compressor_state_ptr, decompressor_state_ptr, &no_block_mode_datas[index]) != 0) {
        return 3;
      }
    }
  }

  return 0;
}

lzws_result_t lzws_test_basic_codes()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_data);
}
