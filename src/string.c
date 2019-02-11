// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "string.h"
#include "buffer.h"
#include "log.h"

// -- compress --

lzws_result_t lzws_compress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups, bool quiet)
{
  lzws_result_t result = lzws_create_buffer_for_compressor(destination_ptr, destination_length_ptr, quiet);
  if (result != 0) {
    return LZWS_STRING_CREATE_BUFFER_FAILED;
  }

  // TODO

  result = lzws_resize_buffer(destination_ptr, *destination_length_ptr, quiet);
  if (result != 0) {
    return LZWS_STRING_RESIZE_BUFFER_FAILED;
  }

  return 0;
}

// -- decompress --

lzws_result_t lzws_decompress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool msb, bool unaligned_bit_groups, bool quiet)
{
  lzws_result_t result = lzws_create_buffer_for_decompressor(destination_ptr, destination_length_ptr, quiet);
  if (result != 0) {
    return LZWS_STRING_CREATE_BUFFER_FAILED;
  }

  // TODO

  result = lzws_resize_buffer(destination_ptr, *destination_length_ptr, quiet);
  if (result != 0) {
    return LZWS_STRING_RESIZE_BUFFER_FAILED;
  }

  return 0;
}
