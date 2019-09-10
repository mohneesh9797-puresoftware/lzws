// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../log.h"
#include "../../macro.h"
#include "../combination.h"
#include "../string_and_file.h"

#include "data.h"

static inline lzws_result_t test_datas(
  lzws_decompressor_state_t* state_ptr,
  size_t buffer_length, va_list LZWS_UNUSED(args))
{
  const char empty_string[] = "";
  uint8_t*   data           = (uint8_t*)empty_string;
  size_t     data_length    = 0;

  uint8_t* result_data;
  size_t   result_data_length;

  lzws_result_t result = lzws_tests_decompress_string_and_file(
    data, data_length,
    &result_data, &result_data_length,
    buffer_length,
    state_ptr->without_magic_header,
    state_ptr->msb,
    state_ptr->unaligned_bit_groups);

  if (result != LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_FAILED) {
    LZWS_LOG_ERROR("empty data should not be decompressed, magic header and header (or header only) are required");
    return 1;
  }

  return 0;
}

lzws_result_t lzws_test_invalid_datas()
{
  return lzws_test_decompressor_combinations(test_datas);
}
