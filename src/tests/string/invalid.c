// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../string.h"

#include "invalid.h"

lzws_result_t lzws_string_test_invalid()
{
  if (lzws_compress_string(NULL, 0, NULL, NULL, 0, LZWS_LOWEST_MAX_CODE_BIT_LENGTH - 1, false, false, false, false) != LZWS_STRING_COMPRESSOR_FAILED) {
    return LZWS_STRING_TEST_INVALID_MAX_CODE_BIT_LENGTH;
  }

  if (lzws_compress_string(NULL, 0, NULL, NULL, 0, LZWS_BIGGEST_MAX_CODE_BIT_LENGTH - 1, false, false, false, false) != LZWS_STRING_COMPRESSOR_FAILED) {
    return LZWS_STRING_TEST_INVALID_MAX_CODE_BIT_LENGTH;
  }

  return 0;
}
