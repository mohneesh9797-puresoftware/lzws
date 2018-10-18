// LZW streaming compressor/decompressor based on LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#ifndef LZW_COMPRESSOR_COMMON_H_
#define LZW_COMPRESSOR_COMMON_H_

#include <stdint.h>

typedef uint8_t lzw_result_t;

extern const uint16_t LZW_MAGIC_HEADER;

#define LZW_LOWEST_MAX_BITS 9
#define LZW_BIGGEST_MAX_BITS 16
#define LZW_MAX_BITS_MASK 0x1f // "max_bits" are the last bits, biggest value (16) requires 5 bits.

#define LZW_BLOCK_MODE 0x80

#define LZW_CLEAR_CODE   256 // Code to flush dictionary and restart decoder.
#define LZW_FIRST_STRING 257 // Code of first dictionary string.

#endif // LZW_COMPRESSOR_COMMON_H_
