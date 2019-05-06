// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

#include "../file.h"
#include "../log.h"
#include "../string.h"

#include "string_and_file.h"

static inline lzws_result_t prepare_files(
  FILE** source_file_ptr, uint8_t* source, size_t source_length,
  FILE** destination_file_ptr, uint8_t** destination_for_file_ptr, size_t destination_length_for_string)
{
  FILE* source_file = fmemopen(source, source_length, "r");
  if (source_file == NULL) {
    LZWS_LOG_ERROR("fmemopen for source failed");
    return LZWS_TEST_STRING_AND_FILE_FMEMOPEN_FAILED;
  }

  // POSIX group don't want users to use binary mode "b" for fmemopen.
  // So we have to give fmemopen ability to write null character.
  size_t destination_length_for_file = destination_length_for_string + 1;

  uint8_t* destination_for_file = malloc(destination_length_for_file);
  if (destination_for_file == NULL) {
    LZWS_LOG_ERROR("malloc failed, buffer length: %zu", destination_length_for_file)

    fclose(source_file);

    return LZWS_TEST_STRING_AND_FILE_ALLOCATE_FAILED;
  }

  FILE* destination_file = fmemopen(destination_for_file, destination_length_for_file, "w");
  if (destination_file == NULL) {
    LZWS_LOG_ERROR("fmemopen for destination failed");

    fclose(source_file);
    free(destination_for_file);

    return LZWS_TEST_STRING_AND_FILE_FMEMOPEN_FAILED;
  }

  *source_file_ptr          = source_file;
  *destination_file_ptr     = destination_file;
  *destination_for_file_ptr = destination_for_file;

  return 0;
}

lzws_result_t lzws_tests_compress_string_and_file(
  uint8_t* source, size_t source_length, size_t source_buffer_length,
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
    LZWS_LOG_ERROR("compress string failed");
    return LZWS_TEST_STRING_AND_FILE_COMPRESSOR_FAILED;
  }

  FILE*    source_file;
  FILE*    destination_file;
  uint8_t* destination_for_file;

  result = prepare_files(
    &source_file, source, source_length,
    &destination_file, &destination_for_file, destination_length_for_string);
  if (result != 0) {
    free(destination_for_string);
    return result;
  }

  result = lzws_compress_file(
    source_file, source_buffer_length,
    destination_file, destination_buffer_length,
    max_code_bit_length, block_mode, msb, unaligned_bit_groups, false);

  fclose(source_file);
  fclose(destination_file);

  if (result != 0) {
    LZWS_LOG_ERROR("compress file failed");

    free(destination_for_string);
    free(destination_for_file);

    return LZWS_TEST_STRING_AND_FILE_COMPRESSOR_FAILED;
  }

  if (strncmp((const char*)destination_for_string, (const char*)destination_for_file, destination_length_for_string) != 0) {
    LZWS_LOG_ERROR("compress string and file received different results");

    free(destination_for_string);
    free(destination_for_file);

    return LZWS_TEST_STRING_AND_FILE_COMPRESSOR_RECEIVED_DIFFERENT_RESULTS;
  }

  free(destination_for_file);

  *destination_ptr        = destination_for_string;
  *destination_length_ptr = destination_length_for_string;

  return result;
}

lzws_result_t lzws_tests_decompress_string_and_file(
  uint8_t* source, size_t source_length, size_t source_buffer_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool msb, bool unaligned_bit_groups)
{
  uint8_t* destination_for_string;
  size_t   destination_length_for_string;

  lzws_result_t result = lzws_decompress_string(
    source, source_length,
    &destination_for_string, &destination_length_for_string, destination_buffer_length,
    msb, unaligned_bit_groups, false);

  if (result != 0) {
    LZWS_LOG_ERROR("decompress string failed");
    return LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_FAILED;
  }

  FILE*    source_file;
  FILE*    destination_file;
  uint8_t* destination_for_file;

  result = prepare_files(
    &source_file, source, source_length,
    &destination_file, &destination_for_file, destination_length_for_string);
  if (result != 0) {
    free(destination_for_string);
    return result;
  }

  result = lzws_decompress_file(
    source_file, source_buffer_length,
    destination_file, destination_buffer_length,
    msb, unaligned_bit_groups, false);

  fclose(source_file);
  fclose(destination_file);

  if (result != 0) {
    LZWS_LOG_ERROR("decompress file failed");

    free(destination_for_string);
    free(destination_for_file);

    return LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_FAILED;
  }

  if (strncmp((const char*)destination_for_string, (const char*)destination_for_file, destination_length_for_string) != 0) {
    LZWS_LOG_ERROR("decompress string and file received different results");

    free(destination_for_string);
    free(destination_for_file);

    result = LZWS_TEST_STRING_AND_FILE_DECOMPRESSOR_RECEIVED_DIFFERENT_RESULTS;
  }

  free(destination_for_file);

  *destination_ptr        = destination_for_string;
  *destination_length_ptr = destination_length_for_string;

  return result;
}
