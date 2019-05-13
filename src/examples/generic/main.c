// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

#include "../../compressor/common.h"
#include "../../compressor/header.h"
#include "../../compressor/main.h"
#include "../../decompressor/common.h"
#include "../../decompressor/header.h"
#include "../../decompressor/main.h"
#include "../../log.h"

#define BUFFER_LENGTH 512

#define MAX_CODE_BIT_LENGTH LZWS_BIGGEST_MAX_CODE_BIT_LENGTH
#define BLOCK_MODE true
#define MSB false
#define UNALIGNED_BIT_GROUPS false
#define QUIET false

int main()
{
  const char text[]      = "example text";
  size_t     text_length = strlen(text);

  char   compressed_text[BUFFER_LENGTH];
  char*  compressed_buffer        = compressed_text;
  size_t compressed_buffer_length = BUFFER_LENGTH;

  lzws_result_t result = lzws_compressor_write_magic_header((uint8_t**)&compressed_buffer, &compressed_buffer_length);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor write magic header failed");
    return 1;
  }

  lzws_compressor_state_t* compressor_state_ptr;

  result = lzws_compressor_get_initial_state(
    &compressor_state_ptr,
    MAX_CODE_BIT_LENGTH, BLOCK_MODE, MSB, UNALIGNED_BIT_GROUPS, QUIET);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor get initial state failed");
    return 2;
  }

  char*  remaining_text        = (char*)text;
  size_t remaining_text_length = text_length;

  result = lzws_compress(compressor_state_ptr, (uint8_t**)&remaining_text, &remaining_text_length, (uint8_t**)&compressed_buffer, &compressed_buffer_length);
  if (result != LZWS_COMPRESSOR_NEEDS_MORE_SOURCE || remaining_text_length != 0) {
    LZWS_LOG_ERROR("compressor failed");

    lzws_compressor_free_state(compressor_state_ptr);

    return 3;
  }

  result = lzws_flush_compressor(compressor_state_ptr, (uint8_t**)&compressed_buffer, &compressed_buffer_length);

  lzws_compressor_free_state(compressor_state_ptr);

  if (result != 0) {
    LZWS_LOG_ERROR("flush compressor failed");
    return 4;
  }

  // decompress

  lzws_decompressor_state_t* decompressor_state_ptr;

  result = lzws_decompressor_get_initial_state(
    &decompressor_state_ptr,
    MSB, UNALIGNED_BIT_GROUPS, QUIET);
  if (result != 0) {
    LZWS_LOG_ERROR("decompressor get initial state failed");
    return 5;
  }

  char*  compressed_text_ptr    = compressed_text;
  size_t compressed_text_length = BUFFER_LENGTH - compressed_buffer_length;

  result = lzws_decompressor_read_magic_header(decompressor_state_ptr, (uint8_t**)&compressed_text_ptr, &compressed_text_length);
  if (result != 0) {
    LZWS_LOG_ERROR("decompressor read magic header failed");

    lzws_decompressor_free_state(decompressor_state_ptr);

    return 6;
  }

  char   decompressed_text[BUFFER_LENGTH];
  char*  decompressed_buffer        = decompressed_text;
  size_t decompressed_buffer_length = BUFFER_LENGTH;

  result = lzws_decompress(decompressor_state_ptr, (uint8_t**)&compressed_text_ptr, &compressed_text_length, (uint8_t**)&decompressed_buffer, &decompressed_buffer_length);

  lzws_decompressor_free_state(decompressor_state_ptr);

  if (result != LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE || compressed_text_length != 0) {
    LZWS_LOG_ERROR("decompressor failed");
    return 7;
  }

  if (strncmp(decompressed_text, text, text_length) != 0) {
    LZWS_LOG_ERROR("decompressed text is not the same as original");
    return 8;
  }

  return 0;
}
