// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../compressor/code.h"
#include "../compressor/header.h"
#include "../compressor/remainder.h"
#include "../log.h"

#include "codes.h"

lzws_result_t lzws_test_compressor_write_codes(
  lzws_compressor_state_t* compressor_state_ptr,
  const lzws_code_t* codes, size_t codes_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  lzws_result_t result = lzws_compressor_write_magic_header(destination_ptr, destination_length_ptr);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor failed to write magic header");
    return result;
  }

  result = lzws_compressor_write_header(compressor_state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor failed to write header");
    return result;
  }

  for (size_t index = 0; index < codes_length; index++) {
    lzws_code_t code = codes[index];

    result = lzws_compressor_write_code(compressor_state_ptr, code, destination_ptr, destination_length_ptr);
    if (result != 0) {
      LZWS_LOG_ERROR("compressor failed to write code");
      return result;
    }
  }

  result = lzws_compressor_flush_remainder(compressor_state_ptr, destination_ptr, destination_length_ptr);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor failed to flush remainder");
    return result;
  }

  return 0;
}
