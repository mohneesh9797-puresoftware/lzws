// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_CONSTANTS_H)
#define LZWS_CONSTANTS_H

#include <stdint.h>

// Standard table to reverse single byte.
extern const uint8_t LZWS_REVERSE_BITS[256];

// Algorithm will try to access 2 ** 0 - 2 ** 16 only.
extern const uint32_t LZWS_POWERS_OF_TWO[17];

// Algorithm will try to access ((2 ** 0) - 1) - ((2 ** 16) - 1) only.
extern const uint16_t LZWS_BIT_MASKS[17];

#endif // LZWS_CONSTANTS_H
