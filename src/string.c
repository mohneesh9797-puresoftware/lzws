// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "compressor/common.h"
#include "compressor/header.h"
#include "compressor/main.h"

#include "decompressor/common.h"
#include "decompressor/header.h"
#include "decompressor/main.h"

#include "buffer.h"
#include "log.h"
#include "string.h"

// -- buffer --

// Destination consist of data and buffer tail.
// Algorithm has written some data into destination buffer.
// We need to increase destination and use its tail as destination buffer.

static inline lzws_result_t increase_destination_buffer(
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  uint8_t** destination_buffer_ptr, size_t* destination_buffer_length_ptr, size_t initial_destination_buffer_length,
  bool quiet)
{
  size_t destination_buffer_length = *destination_buffer_length_ptr;
  if (destination_buffer_length == initial_destination_buffer_length) {
    // We want to write more data at once, than buffer has.
    if (!quiet) {
      LZWS_LOG_ERROR("not enough destination buffer, length: %zu", destination_buffer_length)
    }

    return LZWS_STRING_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  size_t destination_length = *destination_length_ptr;

  lzws_result_t result = lzws_resize_buffer(destination_ptr, destination_length + initial_destination_buffer_length, quiet);
  if (result != 0) {
    return LZWS_STRING_RESIZE_BUFFER_FAILED;
  }

  *destination_buffer_ptr        = *destination_ptr + destination_length;
  *destination_buffer_length_ptr = initial_destination_buffer_length;

  return 0;
}

#define INCREASE_DESTINATION_BUFFER()                                                   \
  result = increase_destination_buffer(                                                 \
    destination_ptr, destination_length_ptr,                                            \
    &destination_buffer, &destination_buffer_length, initial_destination_buffer_length, \
    quiet);                                                                             \
                                                                                        \
  if (result != 0) {                                                                    \
    return result;                                                                      \
  }

static inline void flush_destination_buffer(size_t* destination_length_ptr, size_t destination_buffer_length, size_t initial_destination_buffer_length)
{
  *destination_length_ptr += initial_destination_buffer_length - destination_buffer_length;
}

#define FLUSH_DESTINATION_BUFFER() \
  flush_destination_buffer(destination_length_ptr, destination_buffer_length, initial_destination_buffer_length);

// It is better to wrap function calls that writes something.

#define WITH_WRITE_BUFFER(NEEDS_MORE_SOURCE, NEEDS_MORE_DESTINATION, FAILED, function, ...) \
  while (true) {                                                                            \
    result = (function)(__VA_ARGS__);                                                       \
    if (result == 0 || result == NEEDS_MORE_SOURCE) {                                       \
      FLUSH_DESTINATION_BUFFER()                                                            \
      break;                                                                                \
    }                                                                                       \
    else if (result == NEEDS_MORE_DESTINATION) {                                            \
      FLUSH_DESTINATION_BUFFER()                                                            \
      INCREASE_DESTINATION_BUFFER()                                                         \
    }                                                                                       \
    else {                                                                                  \
      return FAILED;                                                                        \
    }                                                                                       \
  }

static inline lzws_result_t trim_destination_buffer(uint8_t** destination_ptr, size_t destination_length, bool quiet)
{
  lzws_result_t result = lzws_resize_buffer(destination_ptr, destination_length, quiet);
  if (result != 0) {
    return LZWS_STRING_RESIZE_BUFFER_FAILED;
  }

  return 0;
}

// -- compress --

#define COMPRESS_WITH_WRITE_BUFFER(...) \
  WITH_WRITE_BUFFER(LZWS_COMPRESSOR_NEEDS_MORE_SOURCE, LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION, LZWS_STRING_COMPRESSOR_FAILED, __VA_ARGS__)

static inline lzws_result_t compress_data(
  lzws_compressor_state_t* state_ptr,
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, uint8_t* destination_buffer, size_t initial_destination_buffer_length,
  bool quiet)
{
  lzws_result_t result;

  size_t destination_buffer_length = initial_destination_buffer_length;

  COMPRESS_WITH_WRITE_BUFFER(&lzws_compressor_write_magic_header, &destination_buffer, &destination_buffer_length)
  COMPRESS_WITH_WRITE_BUFFER(&lzws_compress, state_ptr, &source, &source_length, &destination_buffer, &destination_buffer_length)
  COMPRESS_WITH_WRITE_BUFFER(&lzws_flush_compressor, state_ptr, &destination_buffer, &destination_buffer_length)

  return trim_destination_buffer(destination_ptr, *destination_length_ptr, quiet);
}

lzws_result_t lzws_compress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups, bool quiet)
{
  uint8_t* destination_buffer;

  lzws_result_t result = lzws_create_buffer_for_compressor(&destination_buffer, &destination_buffer_length, quiet);
  if (result != 0) {
    return LZWS_STRING_CREATE_BUFFER_FAILED;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = 0;

  lzws_compressor_state_t* state_ptr;

  result = lzws_compressor_get_initial_state(&state_ptr, max_code_bit_length, block_mode, msb, unaligned_bit_groups, quiet);
  if (result != 0) {
    free(*destination_ptr);

    return LZWS_STRING_COMPRESSOR_FAILED;
  }

  result = compress_data(
    state_ptr,
    source, source_length,
    destination_ptr, destination_length_ptr, destination_buffer, destination_buffer_length,
    quiet);

  lzws_compressor_free_state(state_ptr);

  if (result != 0) {
    free(*destination_ptr);

    return result;
  }

  return 0;
}

// -- decompress --

#define DECOMPRESS_WITH_WRITE_BUFFER(...) \
  WITH_WRITE_BUFFER(LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE, LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION, LZWS_STRING_DECOMPRESSOR_FAILED, __VA_ARGS__)

static inline lzws_result_t decompress_data(
  lzws_decompressor_state_t* state_ptr,
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, uint8_t* destination_buffer, size_t initial_destination_buffer_length,
  bool quiet)
{
  lzws_result_t result;

  size_t destination_buffer_length = initial_destination_buffer_length;

  DECOMPRESS_WITH_WRITE_BUFFER(&lzws_decompressor_read_magic_header, state_ptr, &source, &source_length);
  DECOMPRESS_WITH_WRITE_BUFFER(&lzws_decompress, state_ptr, &source, &source_length, &destination_buffer, &destination_buffer_length);

  return trim_destination_buffer(destination_ptr, *destination_length_ptr, quiet);
}

lzws_result_t lzws_decompress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool msb, bool unaligned_bit_groups, bool quiet)
{
  uint8_t* destination_buffer;

  lzws_result_t result = lzws_create_buffer_for_decompressor(&destination_buffer, &destination_buffer_length, quiet);
  if (result != 0) {
    return LZWS_STRING_CREATE_BUFFER_FAILED;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = 0;

  lzws_decompressor_state_t* state_ptr;

  result = lzws_decompressor_get_initial_state(&state_ptr, msb, unaligned_bit_groups, quiet);
  if (result != 0) {
    free(*destination_ptr);

    return LZWS_STRING_DECOMPRESSOR_FAILED;
  }

  result = decompress_data(
    state_ptr,
    source, source_length,
    destination_ptr, destination_length_ptr, destination_buffer, destination_buffer_length,
    quiet);

  lzws_decompressor_free_state(state_ptr);

  if (result != 0) {
    free(*destination_ptr);

    return result;
  }

  return 0;
}
