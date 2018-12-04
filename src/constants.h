// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_CONSTANTS_H)
#define LZWS_CONSTANTS_H

#include <stdint.h>

extern const uint8_t  LZWS_REVERSE_BITS_TABLE[256];
extern const uint32_t LZWS_POWERS_OF_TWO[];

#endif // LZWS_CONSTANTS_H
