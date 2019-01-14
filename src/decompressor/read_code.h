// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_READ_CODE_H)
#define LZWS_DECOMPRESSOR_READ_CODE_H

#include "common.h"
#include "state.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_READ_CODE_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

lzws_result_t lzws_decompressor_read_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr);
lzws_result_t lzws_decompressor_read_next_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr);

LZWS_INLINE lzws_result_t lzws_decompressor_verify_empty_source_remainder(lzws_decompressor_state_t* state_ptr) {
  if (state_ptr->source_remainder_bits != 0 && state_ptr->source_remainder != 0) {
    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }
  return 0;
}

#endif // LZWS_DECOMPRESSOR_READ_CODE_H
