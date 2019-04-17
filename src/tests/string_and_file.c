// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <stdio.h>

#include "../file.h"
#include "../string.h"

#include "string_and_file.h"

lzws_result_t lzws_tests_compress_string_and_file(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups)
{
  uint8_t* destination_for_string;
  size_t   destination_length_for_string;

  lzws_result_t result = lzws_compress_string(
    source, source_length,
    &destination_for_string, &destination_length_for_string, destination_buffer_length,
    max_code_bit_length, block_mode, msb, unaligned_bit_groups, false);

  if (result != 0) {
    return LZWS_TEST_STRING_AND_FILE_COMPRESSOR_FAILED;
  }

  uint8_t* destination_for_file = malloc(destination_length_for_string);
  if (destination_for_file == NULL) {
    free(destination_for_string);
  }

  // FILE* source_file = fmemopen(source, source_length, "r");
  // if (source_file == NULL) {
  //   return LZWS_TEST_CODES_FMEMOPEN_FAILED;
  // }
  //
  // lzws_compress_file(
  //   source_file, source_length,
  //   destination_file_ptr, destination_buffer_length,
  //   max_code_bit_length, block_mode, msb, unaligned_bit_groups, false);
  //
  // fclose(source_file);

  return 0;
}

lzws_result_t lzws_tests_decompress_string_and_file(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool msb, bool unaligned_bit_groups)
{
  lzws_result_t result = lzws_decompress_string(
    source, source_length,
    destination_ptr, destination_length_ptr, destination_buffer_length,
    msb, unaligned_bit_groups, false);

  if (result != 0) {
    return LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_FAILED;
  }

  return 0;
}
