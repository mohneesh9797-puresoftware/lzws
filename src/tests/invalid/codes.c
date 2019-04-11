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
  lzws_code_t* codes;
  size_t       length;
} data_t;

static lzws_code_t data0_1[] = {LZWS_FIRST_FREE_CODE};        // First free code can't be clear code.
static lzws_code_t data1_1[] = {LZWS_FIRST_FREE_CODE + 1};    // First code should be a char code.
static lzws_code_t data2_2[] = {1, LZWS_FIRST_FREE_CODE + 1}; // Second code is greater than next code (equals to first free code).

static const data_t datas[] = {
  {data0_1, 1},
  {data1_1, 1},
  {data2_2, 2}};
static const size_t datas_length = sizeof(datas) / sizeof(data_t);

#define SOURCE_LENGTH 6 // 3 bytes for header + 3 bytes for 2 x 9-bit codes.
static uint8_t source[SOURCE_LENGTH];

#define DESTINATION_LENGTH 2 // 2 symbols from 2 x 9-bit codes.
static uint8_t destination[DESTINATION_LENGTH];

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list LZWS_UNUSED(args))
{
  for (size_t index = 0; index < datas_length; index++) {
    const data_t* data_ptr = &datas[index];

    uint8_t* source_ptr    = source;
    size_t   source_length = SOURCE_LENGTH;

    if (
      lzws_compressor_write_magic_header(&source_ptr, &source_length) != 0 ||
      lzws_compressor_write_header(compressor_state_ptr, &source_ptr, &source_length) != 0) {
      return 1;
    }

    for (size_t jndex = 0; jndex < data_ptr->length; jndex++) {
      lzws_code_t code = data_ptr->codes[jndex];

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

    uint8_t* destination_ptr    = destination;
    size_t   destination_length = DESTINATION_LENGTH;

    if (
      lzws_decompress_string(
        source, source_length,
        &destination_ptr, &destination_length, 0,
        decompressor_state_ptr->msb, decompressor_state_ptr->unaligned_bit_groups, decompressor_state_ptr->quiet) != LZWS_STRING_DECOMPRESSOR_FAILED) {
      return 4;
    }
  }

  return 0;
}

lzws_result_t lzws_test_invalid_codes()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_data);
}
