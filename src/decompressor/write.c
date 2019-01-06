// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "write.h"
#include "common.h"

lzws_result_t lzws_decompressor_write_first_symbol(lzws_decompressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  if (*destination_length_ptr < 1) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_write_byte(destination_ptr, destination_length_ptr, state_ptr->prefix_code);

  state_ptr->status = LZWS_DECOMPRESSOR_READ_NEXT_CODE;

  return 0;
}

lzws_result_t lzws_decompressor_write_current_code(lzws_decompressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  if (*destination_length_ptr < 1) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_code_fast_t current_code = state_ptr->current_code;

  // LZWS_ALPHABET_LENGTH

  state_ptr->prefix_code = current_code;
  state_ptr->status      = LZWS_DECOMPRESSOR_READ_NEXT_CODE;

  return 0;
}
