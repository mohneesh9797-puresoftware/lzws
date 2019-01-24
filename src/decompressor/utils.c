// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../utils.h"

#include "utils.h"

void lzws_decompressor_read_byte(lzws_decompressor_state_t* state, uint8_t** source_ptr, size_t* source_length_ptr, uint_fast8_t* byte_ptr) {
  uint_fast8_t byte;
  lzws_read_byte(source_ptr, source_length_ptr, &byte);

  if (state->msb) {
    byte = lzws_get_byte_with_reversed_bits(byte);
  }

  *byte_ptr = byte;
}
