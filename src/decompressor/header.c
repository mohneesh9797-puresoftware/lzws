// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../utils.h"

#include "common.h"
#include "header.h"

lzws_result_t lzws_decompressor_read_magic_header(uint8_t** source_ptr, size_t* source_length_ptr) {
  if (*source_length_ptr < 2) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint_fast8_t byte;
  lzws_read_byte(source_ptr, source_length_ptr, &byte);
  if (byte != LZWS_MAGIC_HEADER_BYTE_0) {
    return LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER;
  }

  lzws_read_byte(source_ptr, source_length_ptr, &byte);
  if (byte != LZWS_MAGIC_HEADER_BYTE_1) {
    return LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER;
  }

  return 0;
}

lzws_result_t lzws_decompressor_read_header(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  // Reading 1 byte for "max_code_bits" and "block_mode".
  if (*source_length_ptr < 1) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint_fast8_t byte;
  lzws_read_byte(source_ptr, source_length_ptr, &byte);

  uint_fast8_t max_code_bits = byte & LZWS_MAX_CODE_BITS_MASK;
  if (max_code_bits < LZWS_LOWEST_MAX_CODE_BITS || max_code_bits > LZWS_BIGGEST_MAX_CODE_BITS) {
    return LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BITS;
  }

  bool block_mode = (byte & LZWS_BLOCK_MODE) != 0;

  state_ptr->max_code_bits = max_code_bits;
  state_ptr->block_mode    = block_mode;

  lzws_code_fast_t initial_used_code = lzws_get_initial_used_code(block_mode);
  lzws_code_fast_t max_code          = lzws_get_bit_mask(max_code_bits);

  state_ptr->initial_used_code   = initial_used_code;
  state_ptr->max_code            = max_code;
  state_ptr->last_used_code      = initial_used_code;
  state_ptr->last_used_code_bits = LZWS_LOWEST_MAX_CODE_BITS;

  state_ptr->status = LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY;

  return 0;
}
