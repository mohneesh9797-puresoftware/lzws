// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_CONSTANTS_H)
#define LZWS_CONSTANTS_H

#include <stdint.h>

// The size of this table is standard to reverse single byte.
extern const uint8_t LZWS_REVERSE_BITS_TABLE[256];

// Algorithm will try to find 2 ** 0 - 2 ** 16 only.
extern const uint32_t LZWS_POWERS_OF_TWO[17];

#endif // LZWS_CONSTANTS_H
