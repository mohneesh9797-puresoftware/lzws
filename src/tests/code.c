// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../compressor/code.h"
#include "../buffer.h"
#include "../compressor/common.h"
#include "../compressor/header.h"
#include "../compressor/remainder.h"
#include "../log.h"

#include "code.h"

// -- buffer --

// Destination consist of data and buffer tail.
// Algorithm has written some data into destination buffer.
// We need to increase destination and use its tail as destination buffer.

static inline void flush_destination_buffer(size_t* destination_length_ptr, size_t destination_buffer_length, size_t initial_destination_buffer_length)
{
  *destination_length_ptr += initial_destination_buffer_length - destination_buffer_length;
}

#define FLUSH_DESTINATION_BUFFER() \
  flush_destination_buffer(destination_length_ptr, destination_buffer_length, initial_destination_buffer_length);

static inline lzws_result_t increase_destination_buffer(
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  uint8_t** destination_buffer_ptr, size_t* destination_buffer_length_ptr, size_t initial_destination_buffer_length)
{
  size_t destination_buffer_length = *destination_buffer_length_ptr;
  if (destination_buffer_length == initial_destination_buffer_length) {
    // We want to write more data at once, than buffer has.
    LZWS_LOG_ERROR("not enough destination buffer, length: %zu", destination_buffer_length)

    return LZWS_TEST_CODES_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  size_t destination_length = *destination_length_ptr;

  lzws_result_t result = lzws_resize_buffer(destination_ptr, destination_length + initial_destination_buffer_length, false);
  if (result != 0) {
    return LZWS_TEST_CODES_RESIZE_BUFFER_FAILED;
  }

  *destination_buffer_ptr        = *destination_ptr + destination_length;
  *destination_buffer_length_ptr = initial_destination_buffer_length;

  return 0;
}

#define INCREASE_DESTINATION_BUFFER()                                                    \
  FLUSH_DESTINATION_BUFFER()                                                             \
                                                                                         \
  result = increase_destination_buffer(                                                  \
    destination_ptr, destination_length_ptr,                                             \
    &destination_buffer, &destination_buffer_length, initial_destination_buffer_length); \
                                                                                         \
  if (result != 0) {                                                                     \
    return result;                                                                       \
  }

static inline lzws_result_t trim_destination_buffer(uint8_t** destination_ptr, size_t destination_length)
{
  lzws_result_t result = lzws_resize_buffer(destination_ptr, destination_length, false);
  if (result != 0) {
    return LZWS_TEST_CODES_RESIZE_BUFFER_FAILED;
  }

  return 0;
}

#define TRIM_DESTINATION_BUFFER() \
  FLUSH_DESTINATION_BUFFER()      \
                                  \
  return trim_destination_buffer(destination_ptr, *destination_length_ptr);

// -- wrapper --

// It is better to wrap function calls that writes something.

#define COMPRESS_WITH_WRITE_BUFFER(function, ...)                     \
  while (true) {                                                      \
    result = (function)(__VA_ARGS__);                                 \
    if (result == 0 || result == LZWS_COMPRESSOR_NEEDS_MORE_SOURCE) { \
      break;                                                          \
    }                                                                 \
    else if (result == LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION) {      \
      INCREASE_DESTINATION_BUFFER()                                   \
    }                                                                 \
    else {                                                            \
      return LZWS_TEST_CODES_COMPRESSOR_FAILED;                       \
    }                                                                 \
  }

// -- compress --

static inline lzws_result_t compress_data(
  lzws_compressor_state_t* compressor_state_ptr,
  const lzws_code_t* codes, size_t codes_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, uint8_t* destination_buffer, size_t initial_destination_buffer_length)
{
  lzws_result_t result;

  size_t destination_buffer_length = initial_destination_buffer_length;

  COMPRESS_WITH_WRITE_BUFFER(&lzws_compressor_write_magic_header, &destination_buffer, &destination_buffer_length)
  COMPRESS_WITH_WRITE_BUFFER(&lzws_compressor_write_header, compressor_state_ptr, &destination_buffer, &destination_buffer_length)

  for (size_t index = 0; index < codes_length; index++) {
    lzws_code_t code = codes[index];

    COMPRESS_WITH_WRITE_BUFFER(&lzws_compressor_write_code, compressor_state_ptr, code, &destination_buffer, &destination_buffer_length)
  }

  COMPRESS_WITH_WRITE_BUFFER(&lzws_compressor_flush_remainder, compressor_state_ptr, &destination_buffer, &destination_buffer_length)

  TRIM_DESTINATION_BUFFER()
}

lzws_result_t lzws_test_compressor_write_codes(
  lzws_compressor_state_t* compressor_state_ptr,
  const lzws_code_t* codes, size_t codes_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length)
{
  uint8_t* destination_buffer;

  lzws_result_t result = lzws_create_buffer_for_compressor(&destination_buffer, &destination_buffer_length, false);
  if (result != 0) {
    return LZWS_TEST_CODES_CREATE_BUFFER_FAILED;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = 0;

  result = compress_data(
    compressor_state_ptr,
    codes, codes_length,
    destination_ptr, destination_length_ptr, destination_buffer, destination_buffer_length);

  if (result != 0) {
    free(*destination_ptr);

    return result;
  }

  return 0;
}
