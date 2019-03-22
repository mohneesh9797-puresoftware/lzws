// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../macro.h"
#include "../combinations.h"

#include "source.h"

typedef struct {
  lzws_code_t* data;
  size_t       length;
} source_t;

static lzws_code_t data_1[] = {LZWS_CLEAR_CODE}; // First code can't be clear code.

static const source_t sources[]     = {{data_1, 1}};
static const size_t   source_length = sizeof(sources) / sizeof(source_t);

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list LZWS_UNUSED(args))
{
  for (size_t index = 0; index < source_length; index++) {
    const source_t* source_ptr = &sources[index];
  }

  return 0;
}

lzws_result_t lzws_test_invalid_source()
{
  return lzws_test_compressor_and_decompressor_combinations(test_data);
}
