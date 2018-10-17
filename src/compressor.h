// LZW streaming compressor based on well documented LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#ifndef LZW_COMPRESSOR_H_
#define LZW_COMPRESSOR_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"

enum {
  LZW_COMPRESSOR_ALLOC_FAILED = 1,
  LZW_COMPRESSOR_INVALID_MAX_BITS,
  LZW_COMPRESSOR_NEEDS_MORE_SRC,
  LZW_COMPRESSOR_NEEDS_MORE_DST
};

typedef struct lzw_compressor_state_t lzw_compressor_state_t;

lzw_result_t lzw_get_initial_compressor_state(lzw_compressor_state_t** state, uint8_t max_bits, bool block_mode);
void         lzw_free_compressor_state(lzw_compressor_state_t* state);

lzw_result_t lzw_compress(lzw_compressor_state_t* state, const uint8_t* src, size_t src_length, uint8_t* dst, size_t* dst_length);

#endif // LZW_COMPRESSOR_H_
