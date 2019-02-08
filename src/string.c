// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "string.h"

lzws_result_t lzws_compress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  uint_fast8_t max_code_bit_length, bool block_mode,
  bool msb, bool unaligned_bit_groups, bool quiet);

lzws_result_t lzws_decompress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr,
  bool msb, bool unaligned_bit_groups, bool quiet);
