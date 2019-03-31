// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../macro.h"
#include "../combinations.h"

#include "codes.h"

typedef struct {
  lzws_code_t* data;
  size_t       length;
} codes_t;

static lzws_code_t data_1[] = {LZWS_CLEAR_CODE}; // First code can't be clear code.

static const codes_t codes[]      = {{data_1, 1}};
static const size_t  codes_length = sizeof(codes) / sizeof(codes_t);

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list LZWS_UNUSED(args))
{
  for (size_t index = 0; index < codes_length; index++) {
    const codes_t* codes_ptr = &codes[index];
  }

  return 0;
}

lzws_result_t lzws_test_invalid_codes()
{
  return lzws_test_compatible_compressor_and_decompressor_combinations(test_data);
}
