// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "compressor/common.h"
#include "compressor/header.h"
#include "compressor/main.h"

#include "file.h"

// -- utils --

static inline lzws_result_t allocate_buffer(uint8_t** result_buffer, size_t* result_buffer_length, size_t default_buffer_length) {
  size_t buffer_length = *result_buffer_length;
  if (buffer_length == 0) {
    buffer_length = default_buffer_length;
  }

  uint8_t* buffer = malloc(buffer_length);
  if (buffer == NULL) {
    return LZWS_FILE_ALLOCATE_FAILED;
  }

  *result_buffer        = buffer;
  *result_buffer_length = buffer_length;

  return 0;
}

static inline lzws_result_t allocate_buffers(uint8_t** result_source_buffer, size_t* result_source_buffer_length, uint8_t** result_destination_buffer, size_t* result_destination_buffer_length) {
  uint8_t* source_buffer;
  size_t   source_buffer_length = *result_source_buffer_length;

  lzws_result_t result = allocate_buffer(&source_buffer, &source_buffer_length, DEFAULT_SOURCE_BUFFER_LENGTH);
  if (result != 0) {
    return result;
  }

  uint8_t* destination_buffer;
  size_t   destination_buffer_length = *result_destination_buffer_length;

  result = allocate_buffer(&destination_buffer, &destination_buffer_length, DEFAULT_DESTINATION_BUFFER_LENGTH);
  if (result != 0) {
    // "source_buffer" was allocated, need to free it.
    free(source_buffer);

    return result;
  }

  *result_source_buffer             = source_buffer;
  *result_source_buffer_length      = source_buffer_length;
  *result_destination_buffer        = destination_buffer;
  *result_destination_buffer_length = destination_buffer_length;

  return 0;
}

// -- writing file --

static inline lzws_result_t write_data(FILE* destination_file, uint8_t* data, size_t data_length) {
  if (fwrite(data, 1, data_length, destination_file) == 0) {
    return LZWS_FILE_WRITE_FAILED;
  }

  return 0;
}

// It is better to wrap function calls that outputs something.
// We can flush destination buffer and provide more destination.

#define WITH_FLUSHING_BUFFER(FAILED, NEEDS_MORE_DESTINATION, function, ...) \
  while (true) {                                                            \
    lzws_result_t result = (function)(__VA_ARGS__);                         \
    if (result == 0) {                                                      \
      break;                                                                \
    }                                                                       \
                                                                            \
    if (result != NEEDS_MORE_DESTINATION) {                                 \
      return FAILED;                                                        \
    }                                                                       \
                                                                            \
    result = flush_destination_buffer(                                      \
        destination_file,                                                   \
        &destination, &destination_length,                                  \
        destination_buffer, destination_buffer_length);                     \
    if (result != 0) {                                                      \
      return result;                                                        \
    }                                                                       \
  }

static inline lzws_result_t flush_destination_buffer(FILE* destination_file, uint8_t** destination, size_t* destination_length, uint8_t* destination_buffer, size_t destination_buffer_length) {
  size_t data_length = destination_buffer_length - *destination_length;
  if (data_length == 0) {
    // We want to write more bytes at once, than buffer has.
    return LZWS_FILE_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  lzws_result_t result = write_data(destination_file, destination_buffer, data_length);
  if (result != 0) {
    return result;
  }

  *destination        = destination_buffer;
  *destination_length = destination_buffer_length;

  return 0;
}

static inline lzws_result_t write_remaining_destination_buffer(FILE* destination_file, uint8_t* destination_buffer, size_t destination_buffer_length, size_t destination_length) {
  size_t data_length = destination_buffer_length - destination_length;
  if (data_length == 0) {
    return 0;
  }

  return write_data(destination_file, destination_buffer, data_length);
}

// -- compress --

#define COMPRESS_WITH_FLUSHING_BUFFER(...) \
  WITH_FLUSHING_BUFFER(LZWS_FILE_COMPRESSOR_FAILED, LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION, __VA_ARGS__)

static inline lzws_result_t compress_data(
    lzws_compressor_state_t* state,
    FILE*                    source_file,
    uint8_t*                 source_buffer,
    size_t                   source_buffer_length,
    FILE*                    destination_file,
    uint8_t*                 destination_buffer,
    size_t                   destination_buffer_length) {
  uint8_t* destination        = destination_buffer;
  size_t   destination_length = destination_buffer_length;

  COMPRESS_WITH_FLUSHING_BUFFER(&lzws_compressor_write_magic_header, &destination, &destination_length);

  while (true) {
    size_t source_length = fread(source_buffer, 1, source_buffer_length, source_file);
    if (source_length == 0) {
      break;
    }

    uint8_t* source = source_buffer;
    COMPRESS_WITH_FLUSHING_BUFFER(&lzws_compress, state, &source, &source_length, &destination, &destination_length);
  }

  COMPRESS_WITH_FLUSHING_BUFFER(&lzws_flush_compressor, state, &destination, &destination_length);

  return write_remaining_destination_buffer(destination_file, destination_buffer, destination_buffer_length, destination_length);
}

lzws_result_t lzws_file_compress(FILE* source_file, size_t source_buffer_length, FILE* destination_file, size_t destination_buffer_length, uint8_t max_code_bits, bool block_mode) {
  uint8_t* source_buffer;
  uint8_t* destination_buffer;

  lzws_result_t result = allocate_buffers(&source_buffer, &source_buffer_length, &destination_buffer, &destination_buffer_length);
  if (result != 0) {
    return result;
  }

  lzws_compressor_state_t* state;
  if (lzws_compressor_get_initial_state(&state, max_code_bits, block_mode) != 0) {
    free(source_buffer);
    free(destination_buffer);
    return LZWS_FILE_COMPRESSOR_FAILED;
  }

  result = compress_data(state, source_file, source_buffer, source_buffer_length, destination_file, destination_buffer, destination_buffer_length);

  lzws_compressor_free_state(state);
  free(source_buffer);
  free(destination_buffer);

  return result;
}

// -- decompress --
// TODO
