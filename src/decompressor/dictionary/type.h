// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_DICTIONARY_TYPE_H)
#define LZWS_DECOMPRESSOR_DICTIONARY_TYPE_H

#include "../../common.h"

typedef struct {
  lzws_code_fast_t codes_length_offset;
  lzws_code_t      undefined_previous_code;

  lzws_code_t* previous_codes;
  uint8_t*     last_symbol_by_codes;
  uint8_t*     output_buffer;
} lzws_decompressor_dictionary_t;

#endif // LZWS_DECOMPRESSOR_DICTIONARY_TYPE_H
