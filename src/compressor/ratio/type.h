// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_RATIO_TYPE_H)
#define LZWS_COMPRESSOR_RATIO_TYPE_H

#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool need_to_clear;

  // Maximum for new source length is 10000.
  // Maximum for new destination length <= 20000 (16 bit code).
  uint16_t new_source_length, new_destination_length;

  // Source length and destination length could overflow.
  mpz_t source_length, destination_length;
} lzws_compressor_ratio_t;

#endif // LZWS_COMPRESSOR_RATIO_TYPE_H
