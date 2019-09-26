// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "file.h"

#include <string.h>

#include "buffer.h"
#include "compressor/common.h"
#include "compressor/main.h"
#include "compressor/state.h"
#include "decompressor/common.h"
#include "decompressor/main.h"
#include "decompressor/state.h"
#include "log.h"

// -- file --

static inline lzws_result_t read_data(FILE* data_file, uint8_t* data_buffer, size_t data_buffer_length, size_t* data_length_ptr, bool quiet)
{
  size_t read_length = fread(data_buffer, 1, data_buffer_length, data_file);
  if (read_length == 0 && feof(data_file)) {
    return LZWS_FILE_READ_FINISHED;
  }

  if (read_length != data_buffer_length && ferror(data_file)) {
    if (!quiet) {
      LZWS_LOG_ERROR("read file failed");
    }

    return LZWS_FILE_READ_FAILED;
  }

  *data_length_ptr = read_length;

  return 0;
}

static inline lzws_result_t write_data(FILE* data_file, uint8_t* data_buffer, size_t data_length, bool quiet)
{
  size_t written_length = fwrite(data_buffer, 1, data_length, data_file);
  if (written_length != data_length) {
    if (!quiet) {
      LZWS_LOG_ERROR("write file failed");
    }

    return LZWS_FILE_WRITE_FAILED;
  }

  return 0;
}

// -- buffer --

// We have read some data from file into source buffer.
// Algorithm has read some data from this buffer.
// We need to move remaining data to the top of the buffer.
// Than we need to read more data from file into remaining part of the buffer.
// Than algorithm can use same buffer again.

static inline lzws_result_t read_source_buffer(
  FILE*    source_file,
  uint8_t* source_buffer, size_t source_buffer_length,
  uint8_t** source_ptr, size_t* source_length_ptr,
  bool quiet)
{
  uint8_t* remaining_data        = *source_ptr;
  size_t   remaining_data_length = *source_length_ptr;

  if (source_buffer != remaining_data) {
    if (remaining_data_length != 0) {
      memmove(source_buffer, remaining_data, remaining_data_length);
    }

    // Remaining data can be accessed even if next code will fail.
    *source_ptr = source_buffer;
  }

  size_t remaining_buffer_length = source_buffer_length - remaining_data_length;
  if (remaining_buffer_length == 0) {
    // We want to read more data at once, than buffer has.
    if (!quiet) {
      LZWS_LOG_ERROR("not enough source buffer, length: %zu", source_buffer_length);
    }

    return LZWS_FILE_NOT_ENOUGH_SOURCE_BUFFER;
  }

  uint8_t* remaining_buffer = source_buffer + remaining_data_length;
  size_t   data_length;

  lzws_result_t result = read_data(source_file, remaining_buffer, remaining_buffer_length, &data_length, quiet);
  if (result != 0) {
    return result;
  }

  *source_length_ptr = remaining_data_length + data_length;

  return 0;
}

// Algorithm has written some data into destination buffer.
// We need to write this data into file.
// Than algorithm can use same buffer again.

static inline lzws_result_t flush_destination_buffer(
  FILE*    destination_file,
  uint8_t* destination_buffer, size_t destination_buffer_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  bool quiet)
{
  size_t data_length = destination_buffer_length - *destination_length_ptr;
  if (data_length == 0) {
    // We want to write more data at once, than buffer has.
    if (!quiet) {
      LZWS_LOG_ERROR("not enough destination buffer, length: %zu", destination_buffer_length);
    }

    return LZWS_FILE_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  lzws_result_t result = write_data(destination_file, destination_buffer, data_length, quiet);
  if (result != 0) {
    return result;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = destination_buffer_length;

  return 0;
}

#define READ_MORE_SOURCE()                                                           \
  result = read_source_buffer(                                                       \
    source_file,                                                                     \
    source_buffer, source_buffer_length,                                             \
    &source, &source_length,                                                         \
    quiet);                                                                          \
                                                                                     \
  if (result == LZWS_FILE_READ_FINISHED) {                                           \
    /* Some UNIX compress implementations writes alignment at the end of the file */ \
    /* So source length here can not be zero here. */                                \
    break;                                                                           \
  }                                                                                  \
  else if (result != 0) {                                                            \
    return result;                                                                   \
  }

#define FLUSH_DESTINATION_BUFFER()                 \
  result = flush_destination_buffer(               \
    destination_file,                              \
    destination_buffer, destination_buffer_length, \
    &destination, &destination_length,             \
    quiet);                                        \
                                                   \
  if (result != 0) {                               \
    return result;                                 \
  }

static inline lzws_result_t write_remaining_destination_buffer(
  FILE*    destination_file,
  uint8_t* destination_buffer, size_t destination_buffer_length, size_t destination_length,
  bool quiet)
{
  size_t data_length = destination_buffer_length - destination_length;
  if (data_length == 0) {
    return 0;
  }

  return write_data(destination_file, destination_buffer, data_length, quiet);
}

// -- compress --

#define BUFFERED_COMPRESS(BREAK_AFTER_SUCCESS, function, ...) \
  while (true) {                                              \
    result = (function)(__VA_ARGS__);                         \
                                                              \
    if (result != 0) {                                        \
      switch (result) {                                       \
        case LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION:          \
          FLUSH_DESTINATION_BUFFER();                         \
          continue;                                           \
        default:                                              \
          return LZWS_FILE_COMPRESSOR_UNEXPECTED_ERROR;       \
      }                                                       \
    }                                                         \
                                                              \
    if (BREAK_AFTER_SUCCESS) {                                \
      break;                                                  \
    }                                                         \
                                                              \
    READ_MORE_SOURCE();                                       \
  }

#define BUFFERED_COMPRESS_ONESHOT(...) \
  BUFFERED_COMPRESS(true, __VA_ARGS__);

#define BUFFERED_COMPRESS_LOOP(...) \
  BUFFERED_COMPRESS(false, __VA_ARGS__);

static inline lzws_result_t compress_data(
  lzws_compressor_state_t* state_ptr,
  FILE* source_file, uint8_t* source_buffer, size_t source_buffer_length,
  FILE* destination_file, uint8_t* destination_buffer, size_t destination_buffer_length,
  bool quiet)
{
  uint8_t* source             = NULL;
  size_t   source_length      = 0;
  uint8_t* destination        = destination_buffer;
  size_t   destination_length = destination_buffer_length;

  lzws_result_t result;

  BUFFERED_COMPRESS_LOOP(&lzws_compress, state_ptr, &source, &source_length, &destination, &destination_length);
  BUFFERED_COMPRESS_ONESHOT(&lzws_compressor_finish, state_ptr, &destination, &destination_length);

  return write_remaining_destination_buffer(destination_file, destination_buffer, destination_buffer_length, destination_length, quiet);
}

lzws_result_t lzws_compress_file(
  FILE* source_file, size_t source_buffer_length,
  FILE* destination_file, size_t destination_buffer_length,
  bool without_magic_header, uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups, bool quiet)
{
  lzws_result_t result;

  uint8_t* source_buffer;

  result = lzws_create_buffer_for_compressor(&source_buffer, &source_buffer_length, quiet);
  if (result != 0) {
    return LZWS_FILE_ALLOCATE_FAILED;
  }

  uint8_t* destination_buffer;

  result = lzws_create_buffer_for_compressor(&destination_buffer, &destination_buffer_length, quiet);
  if (result != 0) {
    free(source_buffer);

    return LZWS_FILE_ALLOCATE_FAILED;
  }

  lzws_compressor_state_t* state_ptr;

  result = lzws_compressor_get_initial_state(
    &state_ptr,
    without_magic_header, max_code_bit_length, block_mode, msb, unaligned_bit_groups, quiet);

  if (result != 0) {
    free(source_buffer);
    free(destination_buffer);

    switch (result) {
      case LZWS_COMPRESSOR_ALLOCATE_FAILED:
        return LZWS_FILE_ALLOCATE_FAILED;
      case LZWS_COMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH:
        return LZWS_FILE_VALIDATE_FAILED;
      default:
        return LZWS_FILE_COMPRESSOR_UNEXPECTED_ERROR;
    }
  }

  result = compress_data(
    state_ptr,
    source_file, source_buffer, source_buffer_length,
    destination_file, destination_buffer, destination_buffer_length,
    quiet);

  lzws_compressor_free_state(state_ptr);
  free(source_buffer);
  free(destination_buffer);

  return result;
}

// -- decompress --

#define BUFFERED_DECOMPRESS(function, ...)                  \
  while (true) {                                            \
    result = (function)(__VA_ARGS__);                       \
                                                            \
    if (result != 0) {                                      \
      switch (result) {                                     \
        case LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION:      \
          FLUSH_DESTINATION_BUFFER();                       \
          continue;                                         \
        case LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER:        \
        case LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH: \
          return LZWS_FILE_VALIDATE_FAILED;                 \
        case LZWS_DECOMPRESSOR_CORRUPTED_SOURCE:            \
          return LZWS_FILE_DECOMPRESSOR_CORRUPTED_SOURCE;   \
        default:                                            \
          return LZWS_FILE_DECOMPRESSOR_UNEXPECTED_ERROR;   \
      }                                                     \
    }                                                       \
                                                            \
    READ_MORE_SOURCE();                                     \
  }

static inline lzws_result_t decompress_data(
  lzws_decompressor_state_t* state_ptr,
  FILE* source_file, uint8_t* source_buffer, size_t source_buffer_length,
  FILE* destination_file, uint8_t* destination_buffer, size_t destination_buffer_length,
  bool quiet)
{
  uint8_t* source             = NULL;
  size_t   source_length      = 0;
  uint8_t* destination        = destination_buffer;
  size_t   destination_length = destination_buffer_length;

  lzws_result_t result;

  BUFFERED_DECOMPRESS(&lzws_decompress, state_ptr, &source, &source_length, &destination, &destination_length);

  return write_remaining_destination_buffer(destination_file, destination_buffer, destination_buffer_length, destination_length, quiet);
}

lzws_result_t lzws_decompress_file(
  FILE* source_file, size_t source_buffer_length,
  FILE* destination_file, size_t destination_buffer_length,
  bool without_magic_header, bool msb, bool unaligned_bit_groups, bool quiet)
{
  lzws_result_t result;

  uint8_t* source_buffer;

  result = lzws_create_buffer_for_decompressor(&source_buffer, &source_buffer_length, quiet);
  if (result != 0) {
    return LZWS_FILE_ALLOCATE_FAILED;
  }

  uint8_t* destination_buffer;

  result = lzws_create_buffer_for_decompressor(&destination_buffer, &destination_buffer_length, quiet);
  if (result != 0) {
    free(source_buffer);

    return LZWS_FILE_ALLOCATE_FAILED;
  }

  lzws_decompressor_state_t* state_ptr;

  result = lzws_decompressor_get_initial_state(
    &state_ptr,
    without_magic_header, msb, unaligned_bit_groups, quiet);

  if (result != 0) {
    free(source_buffer);
    free(destination_buffer);

    switch (result) {
      case LZWS_DECOMPRESSOR_ALLOCATE_FAILED:
        return LZWS_FILE_ALLOCATE_FAILED;
      default:
        return LZWS_FILE_DECOMPRESSOR_UNEXPECTED_ERROR;
    }
  }

  result = decompress_data(
    state_ptr,
    source_file, source_buffer, source_buffer_length,
    destination_file, destination_buffer, destination_buffer_length,
    quiet);

  lzws_decompressor_free_state(state_ptr);
  free(source_buffer);
  free(destination_buffer);

  return result;
}
