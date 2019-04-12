// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_TESTS_CODES_H)
#define LZWS_TESTS_CODES_H

#include "../compressor/state.h"

lzws_result_t lzws_test_compressor_write_codes(
  lzws_compressor_state_t* compressor_state_ptr,
  const lzws_code_t* codes, size_t codes_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr);

#endif // LZWS_TESTS_CODES_H
