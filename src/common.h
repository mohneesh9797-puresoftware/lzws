// LZW streaming compressor/decompressor based on well documented LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#ifndef LZW_COMPRESSOR_COMMON_H_
#define LZW_COMPRESSOR_COMMON_H_

#include <stdint.h>

typedef uint8_t lzw_result_t;

#define LZW_LOWEST_MAX_SYMBOL_SIZE 9
#define LZW_BIGGEST_MAX_SYMBOL_SIZE 12

#endif  // LZW_COMPRESSOR_COMMON_H_
