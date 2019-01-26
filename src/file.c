// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

#include "compressor/common.h"
#include "compressor/header.h"
#include "compressor/main.h"

#include "decompressor/common.h"
#include "decompressor/header.h"
#include "decompressor/main.h"

#include "file.h"
#include "log.h"

// -- utils --

static inline lzws_result_t allocate_buffer(uint8_t** buffer_ptr, size_t* buffer_length_ptr, size_t default_buffer_length, bool quiet)
{
  size_t buffer_length = *buffer_length_ptr;
  if (buffer_length == 0) {
    buffer_length = default_buffer_length;
  }

  uint8_t* buffer = malloc(buffer_length);
  if (buffer == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, buffer length: %zu", buffer_length)
    }

    return LZWS_FILE_ALLOCATE_FAILED;
  }

  *buffer_ptr        = buffer;
  *buffer_length_ptr = buffer_length;

  return 0;
}

static inline lzws_result_t allocate_buffers(
  uint8_t** source_buffer_ptr, size_t* source_buffer_length_ptr,
  uint8_t** destination_buffer_ptr, size_t* destination_buffer_length_ptr,
  size_t default_buffer_length, bool quiet)
{
  uint8_t* source_buffer;
  size_t   source_buffer_length = *source_buffer_length_ptr;

  lzws_result_t result = allocate_buffer(&source_buffer, &source_buffer_length, default_buffer_length, quiet);
  if (result != 0) {
    return result;
  }

  uint8_t* destination_buffer;
  size_t   destination_buffer_length = *destination_buffer_length_ptr;

  result = allocate_buffer(&destination_buffer, &destination_buffer_length, default_buffer_length, quiet);
  if (result != 0) {
    // Source buffer was allocated, need to free it.
    free(source_buffer);

    return result;
  }

  *source_buffer_ptr             = source_buffer;
  *source_buffer_length_ptr      = source_buffer_length;
  *destination_buffer_ptr        = destination_buffer;
  *destination_buffer_length_ptr = destination_buffer_length;

  return 0;
}

#define ALLOCATE_BUFFERS(DEFAULT_BUFFER_LENGTH)      \
  result = allocate_buffers(                         \
    &source_buffer, &source_buffer_length,           \
    &destination_buffer, &destination_buffer_length, \
    DEFAULT_BUFFER_LENGTH, quiet);                   \
                                                     \
  if (result != 0) {                                 \
    return result;                                   \
  }

// -- files --

static inline lzws_result_t read_data(FILE* data_file_ptr, uint8_t* data_buffer, size_t data_buffer_length, size_t* data_length_ptr, bool quiet)
{
  size_t read_length = fread(data_buffer, 1, data_buffer_length, data_file_ptr);
  if (read_length == 0 && feof(data_file_ptr)) {
    return LZWS_FILE_READ_FINISHED;
  }

  if (read_length != data_buffer_length && ferror(data_file_ptr)) {
    if (!quiet) {
      LZWS_LOG_ERROR("read file failed")
    }

    return LZWS_FILE_READ_FAILED;
  }

  *data_length_ptr = read_length;

  return 0;
}

static inline lzws_result_t write_data(FILE* data_file_ptr, uint8_t* data_buffer, size_t data_length, bool quiet)
{
  size_t written_length = fwrite(data_buffer, 1, data_length, data_file_ptr);
  if (written_length != data_length) {
    if (!quiet) {
      LZWS_LOG_ERROR("write file failed")
    }

    return LZWS_FILE_WRITE_FAILED;
  }

  return 0;
}

// -- buffers --

// We have source buffer, we read some data from file into it.
// Algorithm read some data from buffer.
// We need to move remaining data to the top of the buffer.
// Than we need to read more data from file into remaining part of the buffer.
// Than algorithm can use same buffer again.
static inline lzws_result_t read_source_buffer(
  FILE*    source_file_ptr,
  uint8_t* source_buffer, size_t source_buffer_length,
  uint8_t** source_ptr, size_t* source_length_ptr,
  bool quiet)
{
  uint8_t* remaining_data        = *source_ptr;
  size_t   remaining_data_length = *source_length_ptr;

  if (remaining_data != source_buffer && remaining_data_length != 0) {
    memmove(source_buffer, remaining_data, remaining_data_length);
  }

  uint8_t* remaining_buffer        = source_buffer + remaining_data_length;
  size_t   remaining_buffer_length = source_buffer_length - remaining_data_length;

  if (remaining_buffer_length == 0) {
    // We want to read more data at once, than buffer has.
    if (!quiet) {
      LZWS_LOG_ERROR("not enough source buffer, length: %zu", source_buffer_length)
    }

    return LZWS_FILE_NOT_ENOUGH_SOURCE_BUFFER;
  }

  size_t data_length;

  lzws_result_t result = read_data(source_file_ptr, remaining_buffer, remaining_buffer_length, &data_length, quiet);
  if (result != 0) {
    return result;
  }

  *source_ptr        = source_buffer;
  *source_length_ptr = data_length + remaining_data_length;

  return 0;
}

// We have destination buffer.
// Algorithm has written some data into it.
// We need to write this data into file.
// Than algorithm can use same buffer again.
static inline lzws_result_t flush_destination_buffer(
  FILE*    destination_file_ptr,
  uint8_t* destination_buffer, size_t destination_buffer_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  bool quiet)
{
  size_t data_length = destination_buffer_length - *destination_length_ptr;
  if (data_length == 0) {
    // We want to write more data at once, than buffer has.
    if (!quiet) {
      LZWS_LOG_ERROR("not enough destination buffer, length: %zu", destination_buffer_length)
    }

    return LZWS_FILE_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  lzws_result_t result = write_data(destination_file_ptr, destination_buffer, data_length, quiet);
  if (result != 0) {
    return result;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = destination_buffer_length;

  return 0;
}

// It is better to wrap function calls that reads and writes something.

// We can read more source from file.
#define READ_MORE_SOURCE(FAILED)                      \
  result = read_source_buffer(                        \
    source_file_ptr,                                  \
    source_buffer, source_buffer_length,              \
    &source, &source_length,                          \
    quiet);                                           \
                                                      \
  if (result == LZWS_FILE_READ_FINISHED) {            \
    if (source_length != 0) {                         \
      /* Algorithm is not able to read all source. */ \
      /* It means that algorithm is broken. */        \
      if (!quiet) {                                   \
        LZWS_LOG_ERROR("not able to read all source") \
      }                                               \
                                                      \
      return FAILED;                                  \
    }                                                 \
                                                      \
    break;                                            \
  }                                                   \
  else if (result != 0) {                             \
    return result;                                    \
  }

// We can flush destination buffer into file.
#define FLUSH_DESTINATION_BUFFER()                 \
  result = flush_destination_buffer(               \
    destination_file_ptr,                          \
    destination_buffer, destination_buffer_length, \
    &destination, &destination_length,             \
    quiet);                                        \
                                                   \
  if (result != 0) {                               \
    return result;                                 \
  }

#define WITH_READ_WRITE_BUFFERS(NEEDS_MORE_SOURCE, NEEDS_MORE_DESTINATION, FAILED, function, ...) \
  while (true) {                                                                                  \
    result = (function)(__VA_ARGS__);                                                             \
    if (result == 0) {                                                                            \
      break;                                                                                      \
    }                                                                                             \
    else if (result == NEEDS_MORE_SOURCE) {                                                       \
      READ_MORE_SOURCE(FAILED)                                                                    \
    }                                                                                             \
    else if (result == NEEDS_MORE_DESTINATION) {                                                  \
      FLUSH_DESTINATION_BUFFER()                                                                  \
    }                                                                                             \
    else {                                                                                        \
      return FAILED;                                                                              \
    }                                                                                             \
  }

static inline lzws_result_t write_remaining_destination_buffer(
  FILE*    destination_file_ptr,
  uint8_t* destination_buffer, size_t destination_buffer_length, size_t destination_length,
  bool quiet)
{
  size_t data_length = destination_buffer_length - destination_length;
  if (data_length == 0) {
    return 0;
  }

  return write_data(destination_file_ptr, destination_buffer, data_length, quiet);
}

// -- compress --

#define COMPRESS_WITH_READ_WRITE_BUFFERS(...) \
  WITH_READ_WRITE_BUFFERS(LZWS_COMPRESSOR_NEEDS_MORE_SOURCE, LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION, LZWS_FILE_COMPRESSOR_FAILED, __VA_ARGS__)

static inline lzws_result_t compress_data(
  lzws_compressor_state_t* state_ptr,
  FILE* source_file_ptr, uint8_t* source_buffer, size_t source_buffer_length,
  FILE* destination_file_ptr, uint8_t* destination_buffer, size_t destination_buffer_length,
  bool quiet)
{
  uint8_t* source             = NULL;
  size_t   source_length      = 0;
  uint8_t* destination        = destination_buffer;
  size_t   destination_length = destination_buffer_length;

  lzws_result_t result;

  COMPRESS_WITH_READ_WRITE_BUFFERS(&lzws_compressor_write_magic_header, &destination, &destination_length)
  COMPRESS_WITH_READ_WRITE_BUFFERS(&lzws_compress, state_ptr, &source, &source_length, &destination, &destination_length)
  COMPRESS_WITH_READ_WRITE_BUFFERS(&lzws_flush_compressor, state_ptr, &destination, &destination_length)

  return write_remaining_destination_buffer(destination_file_ptr, destination_buffer, destination_buffer_length, destination_length, quiet);
}

lzws_result_t lzws_file_compress(
  FILE* source_file_ptr, size_t source_buffer_length,
  FILE* destination_file_ptr, size_t destination_buffer_length,
  uint_fast8_t max_code_bit_length, bool block_mode,
  bool msb, bool quiet, bool unaligned_bit_groups)
{
  uint8_t* source_buffer;
  uint8_t* destination_buffer;

  lzws_result_t result;

  ALLOCATE_BUFFERS(LZWS_COMPRESSOR_DEFAULT_BUFFER_LENGTH)

  lzws_compressor_state_t* state_ptr;
  if (lzws_compressor_get_initial_state(&state_ptr, max_code_bit_length, block_mode, msb, quiet, unaligned_bit_groups) != 0) {
    free(source_buffer);
    free(destination_buffer);

    return LZWS_FILE_COMPRESSOR_FAILED;
  }

  result = compress_data(
    state_ptr,
    source_file_ptr, source_buffer, source_buffer_length,
    destination_file_ptr, destination_buffer, destination_buffer_length,
    quiet);

  lzws_compressor_free_state(state_ptr);
  free(source_buffer);
  free(destination_buffer);

  return result;
}

// -- decompress --

#define DECOMPRESS_WITH_READ_WRITE_BUFFERS(...) \
  WITH_READ_WRITE_BUFFERS(LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE, LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION, LZWS_FILE_DECOMPRESSOR_FAILED, __VA_ARGS__)

static inline lzws_result_t decompress_data(
  lzws_decompressor_state_t* state_ptr,
  FILE* source_file_ptr, uint8_t* source_buffer, size_t source_buffer_length,
  FILE* destination_file_ptr, uint8_t* destination_buffer, size_t destination_buffer_length,
  bool quiet)
{
  uint8_t* source             = NULL;
  size_t   source_length      = 0;
  uint8_t* destination        = destination_buffer;
  size_t   destination_length = destination_buffer_length;

  lzws_result_t result;

  DECOMPRESS_WITH_READ_WRITE_BUFFERS(&lzws_decompressor_read_magic_header, state_ptr, &source, &source_length);
  DECOMPRESS_WITH_READ_WRITE_BUFFERS(&lzws_decompress, state_ptr, &source, &source_length, &destination, &destination_length);
  DECOMPRESS_WITH_READ_WRITE_BUFFERS(&lzws_flush_decompressor, state_ptr);

  return write_remaining_destination_buffer(destination_file_ptr, destination_buffer, destination_buffer_length, destination_length, quiet);
}

lzws_result_t lzws_file_decompress(
  FILE* source_file_ptr, size_t source_buffer_length,
  FILE* destination_file_ptr, size_t destination_buffer_length,
  bool msb, bool quiet, bool unaligned_bit_groups)
{
  uint8_t* source_buffer;
  uint8_t* destination_buffer;

  lzws_result_t result;

  ALLOCATE_BUFFERS(LZWS_DECOMPRESSOR_DEFAULT_BUFFER_LENGTH)

  lzws_decompressor_state_t* state_ptr;
  if (lzws_decompressor_get_initial_state(&state_ptr, msb, quiet, unaligned_bit_groups) != 0) {
    free(source_buffer);
    free(destination_buffer);

    return LZWS_FILE_DECOMPRESSOR_FAILED;
  }

  result = decompress_data(
    state_ptr,
    source_file_ptr, source_buffer, source_buffer_length,
    destination_file_ptr, destination_buffer, destination_buffer_length,
    quiet);

  lzws_decompressor_free_state(state_ptr);
  free(source_buffer);
  free(destination_buffer);

  return result;
}
