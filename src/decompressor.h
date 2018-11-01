// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_DECOMPRESSOR_H_
#define LZWS_DECOMPRESSOR_H_

#include <stdint.h>
#include <stdlib.h>

#include "common.h"

enum {
  LZWS_DECOMPRESSOR_NEEDS_MORE_INPUT = 1,
  LZWS_DECOMPRESSOR_NEEDS_MORE_OUTPUT,
  LZWS_DECOMPRESSOR_ALLOC_FAILED
};

typedef struct lzws_decompressor_state_t lzws_decompressor_state_t;

lzws_result_t lzws_get_initial_decompressor_state(lzws_decompressor_state_t** state);
void          lzws_free_decompressor_state(lzws_decompressor_state_t* state);

lzws_result_t lzws_decompress(lzws_decompressor_state_t* state,
                              const uint8_t* src, size_t src_length,
                              uint8_t* dst, size_t* dst_length);

#endif // LZWS_DECOMPRESSOR_H_
