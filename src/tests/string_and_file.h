// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_TESTS_STRING_AND_FILE_H)
#define LZWS_TESTS_STRING_AND_FILE_H

#include <stdlib.h>

#include "../common.h"

// Possible results:
enum {
  LZWS_TEST_STRING_AND_FILE_ALLOCATE_FAILED = 1,
  LZWS_TEST_STRING_AND_FILE_FMEMOPEN_FAILED,
  LZWS_TEST_STRING_AND_FILE_FOPEN_FAILED,
  LZWS_TEST_STRING_AND_FILE_API_FAILED,
  LZWS_TEST_STRING_AND_FILE_COMPRESSOR_FAILED,
  LZWS_TEST_STRING_AND_FILE_COMPRESSOR_IS_VOLATILE,
  LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_FAILED,
  LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_IS_VOLATILE
};

// We can test both string and file api.
// It is possible to run every test part using 2 apis.

lzws_result_t lzws_tests_compress_string_and_file(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups,
  size_t buffer_length);

lzws_result_t lzws_tests_decompress_string_and_file(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  bool msb, bool unaligned_bit_groups,
  size_t buffer_length);

#endif // LZWS_TESTS_STRING_AND_FILE_H