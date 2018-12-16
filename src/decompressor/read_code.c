// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "read_code.h"
#include "common.h"
#include "utils.h"

lzws_result_t lzws_decompressor_read_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  uint_fast8_t code_bits      = state_ptr->last_used_code_bits;
  uint_fast8_t remainder_bits = state_ptr->remainder_bits;

  // Code bits here will always be >= 8.
  // So source bytes will always be >= 1.
  uint_fast8_t source_bytes = lzws_ceil_bits_to_bytes(code_bits + remainder_bits);
  if (*source_length_ptr < source_bytes) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  return 0;
}
