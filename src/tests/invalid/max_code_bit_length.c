// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../compressor/common.h"
#include "../../compressor/state.h"
#include "../../file.h"
#include "../../string.h"

#include "max_code_bit_length.h"

#define MAX_CODE_BIT_LENGTH_SIZE sizeof(max_code_bit_lengths) / sizeof(uint8_t)
static const uint8_t max_code_bit_lengths[] = {
  LZWS_LOWEST_MAX_CODE_BIT_LENGTH - 1,
  LZWS_BIGGEST_MAX_CODE_BIT_LENGTH + 1};

lzws_result_t lzws_test_invalid_max_code_bit_length()
{
  for (size_t index = 0; index < MAX_CODE_BIT_LENGTH_SIZE; index++) {
    uint8_t max_code_bit_length = max_code_bit_lengths[index];

    lzws_compressor_state_t* state_ptr;

    if (lzws_compressor_get_initial_state(&state_ptr, max_code_bit_length, false, false, false, false) != LZWS_COMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH) {
      return 1;
    }

    if (lzws_compress_file(NULL, 0, NULL, 0, max_code_bit_length, false, false, false, false) != LZWS_FILE_COMPRESSOR_FAILED) {
      return 2;
    }

    uint8_t* destination;
    size_t   destination_length;

    if (lzws_compress_string(NULL, 0, &destination, &destination_length, 0, max_code_bit_length, false, false, false, false) != LZWS_STRING_COMPRESSOR_FAILED) {
      return 3;
    }
  }

  return 0;
}
