// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_BUFFER_H)
#define LZWS_BUFFER_H

#include <stdlib.h>

#include "common.h"

// Possible results:
enum {
  LZWS_BUFFER_ALLOCATE_FAILED = 1,
  LZWS_BUFFER_REALLOCATE_FAILED
};

// Compressor/decompressor dictionary implementations has different speed.
// Buffers shouldn't be a bottleneck.
// So we can use bigger buffers for faster implementation.

#if defined(LZWS_COMPRESSOR_DICTIONARY_LINKED_LIST)
#define LZWS_COMPRESSOR_DEFAULT_BUFFER_LENGTH (1 << 15) // 32 KB
#elif defined(LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY)
#define LZWS_COMPRESSOR_DEFAULT_BUFFER_LENGTH (1 << 16) // 64 KB
#endif

#define LZWS_DECOMPRESSOR_DEFAULT_BUFFER_LENGTH (1 << 16) // 64 KB

lzws_result_t lzws_create_buffer_for_compressor(uint8_t** buffer_ptr, size_t* buffer_length_ptr, bool quiet);
lzws_result_t lzws_create_buffer_for_decompressor(uint8_t** buffer_ptr, size_t* buffer_length_ptr, bool quiet);

lzws_result_t lzws_resize_buffer(uint8_t** buffer_ptr, size_t buffer_length, bool quiet);

#endif // LZWS_BUFFER_H
