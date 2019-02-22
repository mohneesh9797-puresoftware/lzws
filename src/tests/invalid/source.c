// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../compressor/state.h"
#include "../../decompressor/state.h"

#include "source.h"

#include <stdlib.h>

typedef struct {
  lzws_code_t* data;
  size_t       length;
} source_t;

static lzws_code_t data_1[] = {LZWS_CLEAR_CODE}; // First code can't be clear code.

static const source_t sources[]     = {{data_1, 1}};
static const size_t   source_length = 1;

lzws_result_t test_data(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr)
{
  for (size_t index = 0; index < source_length; index++) {
    const source_t* source_ptr = &sources[index];
  }

  return 0;
}

lzws_result_t lzws_test_invalid_source()
{
  lzws_compressor_state_t* compressor_state_ptr;

  if (lzws_compressor_get_initial_state(&compressor_state_ptr, LZWS_LOWEST_MAX_CODE_BIT_LENGTH, false, false, false, false) != 0) {
    return 1;
  }

  lzws_decompressor_state_t* decompressor_state_ptr;

  if (lzws_decompressor_get_initial_state(&decompressor_state_ptr, false, false, false) != 0) {
    lzws_compressor_free_state(compressor_state_ptr);

    return 2;
  }

  lzws_result_t result = test_data(compressor_state_ptr, decompressor_state_ptr);

  lzws_compressor_free_state(compressor_state_ptr);
  lzws_decompressor_free_state(decompressor_state_ptr);

  return result;
}
