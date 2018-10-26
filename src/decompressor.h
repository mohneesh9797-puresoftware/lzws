// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZW_DECOMPRESSOR_H_
#define LZW_DECOMPRESSOR_H_

#include <stdint.h>
#include <stdlib.h>

#include "common.h"

enum {
  LZW_DECOMPRESSOR_NEEDS_MORE_INPUT = 1,
  LZW_DECOMPRESSOR_NEEDS_MORE_OUTPUT,
  LZW_DECOMPRESSOR_ALLOC_FAILED
};

typedef struct lzw_decompressor_state_t lzw_decompressor_state_t;

lzw_result_t lzw_get_initial_decompressor_state(lzw_decompressor_state_t** state);
void         lzw_free_decompressor_state(lzw_decompressor_state_t* state);

lzw_result_t lzw_decompress(lzw_decompressor_state_t* state,
                            const uint8_t* src, size_t src_length,
                            uint8_t* dst, size_t* dst_length);

#endif // LZW_DECOMPRESSOR_H_
