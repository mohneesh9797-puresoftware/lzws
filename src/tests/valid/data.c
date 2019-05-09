// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

#include "../../log.h"
#include "../../macro.h"
#include "../combination.h"
#include "../string_and_file.h"

#include "data.h"

static const char* datas[] = {
  "hello world",
  "tobeornottobeortobeornot",
  "qqqqqqqqqqqqqqqqqqqqqqqq",
  "qqqqqqqqqqqqqqqqqqqqqqqz"};
#define DATA_LENGTH 4

static inline lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, const char* data, size_t buffer_length)
{
  size_t data_length = strlen(data);

  uint8_t* compressed_data;
  size_t   compressed_data_length;

  lzws_result_t result = lzws_tests_compress_string_and_file(
    (uint8_t*)data, data_length,
    &compressed_data, &compressed_data_length,
    compressor_state_ptr->max_code_bit_length, compressor_state_ptr->block_mode, compressor_state_ptr->msb, compressor_state_ptr->unaligned_bit_groups,
    buffer_length);
  if (result != 0) {
    return 1;
  }

  char*  result_data;
  size_t result_data_length;

  result = lzws_tests_decompress_string_and_file(
    compressed_data, compressed_data_length,
    (uint8_t**)&result_data, &result_data_length,
    decompressor_state_ptr->msb, decompressor_state_ptr->unaligned_bit_groups,
    buffer_length);

  free(compressed_data);

  if (result != 0) {
    return 2;
  }

  if (result_data_length != data_length) {
    LZWS_LOG_ERROR("decompressed invalid data length %zu, original length %zu", result_data_length, data_length);
    result = 3;
  }
  else if (strncmp(result_data, data, data_length) != 0) {
    LZWS_LOG_ERROR("decompressed data is not the same as original");
    result = 4;
  }
  else {
    result = 0;
  }

  free(result_data);

  return result;
}

static inline lzws_result_t test_datas(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, size_t buffer_length, va_list LZWS_UNUSED(args))
{
  lzws_result_t result;

  for (size_t index = 0; index < DATA_LENGTH; index++) {
    result = test_data(compressor_state_ptr, decompressor_state_ptr, datas[index], buffer_length);
    if (result != 0) {
      return result;
    }
  }

  return 0;
}

lzws_result_t lzws_test_valid_datas()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_datas);
}
