// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "header.h"
#include "common.h"
#include "utils.h"

lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length) {
  if (*destination_length < 2) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_0);
  lzws_write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_1);

  return 0;
}

lzws_result_t lzws_compressor_write_header(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  // Writing 1 byte for "max_code_bits" and "block_mode".
  if (*destination_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  uint8_t byte = state->max_code_bits;
  if (state->block_mode) {
    byte = byte | LZWS_BLOCK_MODE;
  }

  lzws_compressor_write_byte(state, destination, destination_length, byte);

  state->status = LZWS_COMPRESSOR_ALLOCATE_DICTIONARY;

  return 0;
}
