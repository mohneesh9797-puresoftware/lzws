// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_DICTIONARY_COMMON_H)
#define LZWS_DECOMPRESSOR_DICTIONARY_COMMON_H

#include "../../common.h"

// It is possible to use zero instead of max code as undefined previous code.
// We need just to increase each previous code value by 1 (offset constant).

#define LZWS_DECOMPRESSOR_DICTIONARY_PREVIOUS_CODE_OFFSET 1

#define LZWS_DECOMPRESSOR_DICTIONARY_UNDEFINED_PREVIOUS_CODE 0
#define LZWS_DECOMPRESSOR_DICTIONARY_UNDEFINED_PREVIOUS_CODE_ZERO true
#define LZWS_DECOMPRESSOR_DICTIONARY_UNDEFINED_PREVIOUS_CODE_IDENTICAL_BYTES true

typedef struct {
  lzws_code_fast_t codes_length_offset;

  lzws_code_t* previous_codes;
  uint8_t*     last_symbol_by_codes;
  uint8_t*     output_buffer;
} lzws_decompressor_dictionary_t;

#endif // LZWS_DECOMPRESSOR_DICTIONARY_COMMON_H
