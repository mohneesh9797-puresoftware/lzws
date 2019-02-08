// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_FILE_H)
#define LZWS_FILE_H

#include <stdio.h>

#include "common.h"

// Possible results:
enum {
  LZWS_FILE_ALLOCATE_FAILED = 1,
  LZWS_FILE_COMPRESSOR_FAILED,
  LZWS_FILE_DECOMPRESSOR_FAILED,
  LZWS_FILE_NOT_ENOUGH_SOURCE_BUFFER,
  LZWS_FILE_NOT_ENOUGH_DESTINATION_BUFFER,
  LZWS_FILE_READ_FINISHED,
  LZWS_FILE_READ_FAILED,
  LZWS_FILE_WRITE_FAILED
};

// Compressor dictionary implementations has different speed.
// Buffers shouldn't be a bottleneck.
// So we can use bigger buffers for faster implementation.

#if defined(LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST)
#define LZWS_FILE_COMPRESSOR_DEFAULT_BUFFER_LENGTH (1 << 15) // 32 KB
#elif defined(LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY)
#define LZWS_FILE_COMPRESSOR_DEFAULT_BUFFER_LENGTH (1 << 16) // 64 KB
#endif

#define LZWS_FILE_DECOMPRESSOR_DEFAULT_BUFFER_LENGTH (1 << 16) // 64 KB

// Source buffer length and destination buffer length can be equal to 0.
// In this case it will be replaced with default values.

lzws_result_t lzws_file_compress(
  FILE* source_file_ptr, size_t source_buffer_length,
  FILE* destination_file_ptr, size_t destination_buffer_length,
  uint_fast8_t max_code_bit_length, bool block_mode,
  bool msb, bool quiet, bool unaligned_bit_groups);

lzws_result_t lzws_file_decompress(
  FILE* source_file_ptr, size_t source_buffer_length,
  FILE* destination_file_ptr, size_t destination_buffer_length,
  bool msb, bool quiet, bool unaligned_bit_groups);

#endif // LZWS_FILE_H
