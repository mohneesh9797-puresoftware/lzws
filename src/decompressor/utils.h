// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_UTILS_H)
#define LZWS_DECOMPRESSOR_UTILS_H

#include "../utils.h"

#include "state.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_UTILS_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_decompressor_read_byte(lzws_decompressor_state_t* state, uint8_t** source_ptr, size_t* source_length_ptr, uint_fast8_t* byte_ptr) {
  uint_fast8_t byte;
  lzws_read_byte(source_ptr, source_length_ptr, &byte);

  if (state->msb) {
    byte = lzws_reverse_byte(byte);
  }

  *byte_ptr = byte;
}

#endif // LZWS_DECOMPRESSOR_UTILS_H
