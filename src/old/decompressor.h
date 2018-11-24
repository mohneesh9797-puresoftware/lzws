// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_H)
#define LZWS_DECOMPRESSOR_H

#include <stdint.h>
#include <stdlib.h>

#include "common.h"

// Possible results:
enum {
  LZWS_DECOMPRESSOR_ALLOCATE_FAILED = 1,
  LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE,
  LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION
};

typedef struct lzws_decompressor_state_t {
} lzws_decompressor_state_t;

lzws_result_t lzws_get_initial_decompressor_state(lzws_decompressor_state_t** state, uint8_t max_code_bits, bool block_mode);
void          lzws_free_decompressor_state(lzws_decompressor_state_t* state);

lzws_result_t lzws_decompress(lzws_decompressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length);

#endif // LZWS_DECOMPRESSOR_H
