// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "buffer.h"

#include "log.h"

static inline lzws_result_t create_buffer(uint8_t** buffer_ptr, size_t* buffer_length_ptr, size_t default_buffer_length, bool quiet)
{
  size_t buffer_length = *buffer_length_ptr;
  if (buffer_length == 0) {
    buffer_length = default_buffer_length;
  }

  uint8_t* buffer = malloc(buffer_length);
  if (buffer == NULL) {
    if (!quiet) {
      LZWS_LOG_ERROR("malloc failed, buffer length: %zu", buffer_length);
    }

    return LZWS_BUFFER_ALLOCATE_FAILED;
  }

  *buffer_ptr        = buffer;
  *buffer_length_ptr = buffer_length;

  return 0;
}

lzws_result_t lzws_create_buffer_for_compressor(uint8_t** buffer_ptr, size_t* buffer_length_ptr, bool quiet)
{
  return create_buffer(buffer_ptr, buffer_length_ptr, LZWS_COMPRESSOR_DEFAULT_BUFFER_LENGTH, quiet);
}

lzws_result_t lzws_create_buffer_for_decompressor(uint8_t** buffer_ptr, size_t* buffer_length_ptr, bool quiet)
{
  return create_buffer(buffer_ptr, buffer_length_ptr, LZWS_DECOMPRESSOR_DEFAULT_BUFFER_LENGTH, quiet);
}

lzws_result_t lzws_resize_buffer(uint8_t** buffer_ptr, size_t buffer_length, bool quiet)
{
  uint8_t* new_buffer = realloc(*buffer_ptr, buffer_length);
  if (new_buffer == NULL && buffer_length != 0) {
    if (!quiet) {
      LZWS_LOG_ERROR("realloc failed, buffer length: %zu", buffer_length);
    }

    return LZWS_BUFFER_REALLOCATE_FAILED;
  }

  *buffer_ptr = new_buffer;

  return 0;
}
