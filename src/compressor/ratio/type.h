// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_RATIO_TYPE_H)
#define LZWS_COMPRESSOR_RATIO_TYPE_H

#include <gmp.h>
#include <stdint.h>

typedef struct {
  uint32_t new_source_length, new_destination_length;
  mpz_t    source_length, destination_length;
} lzws_compressor_ratio_t;

#endif // LZWS_COMPRESSOR_RATIO_TYPE_H
