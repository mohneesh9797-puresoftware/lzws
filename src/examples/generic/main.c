// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <string.h>

#include "../../buffer.h"
#include "../../compressor/common.h"
#include "../../compressor/header.h"
#include "../../compressor/main.h"
#include "../../compressor/state.h"
#include "../../decompressor/common.h"
#include "../../decompressor/header.h"
#include "../../decompressor/main.h"
#include "../../decompressor/state.h"
#include "../../log.h"

#define MAX_CODE_BIT_LENGTH LZWS_BIGGEST_MAX_CODE_BIT_LENGTH
#define BLOCK_MODE true
#define MSB false
#define UNALIGNED_BIT_GROUPS false
#define QUIET false

int main()
{
  const char text[]      = "example text";
  size_t     text_length = strlen(text);

  uint8_t* compressor_buffer;
  size_t   compressor_buffer_length = 0;

  lzws_result_t result = lzws_create_buffer_for_compressor(&compressor_buffer, &compressor_buffer_length, QUIET);
  if (result != 0) {
    LZWS_LOG_ERROR("create buffer for compressor failed");
    return 1;
  }

  uint8_t* remaining_compressor_buffer        = compressor_buffer;
  size_t   remaining_compressor_buffer_length = compressor_buffer_length;

  result = lzws_compressor_write_magic_header(&remaining_compressor_buffer, &remaining_compressor_buffer_length);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor write magic header failed");

    free(compressor_buffer);

    return 2;
  }

  lzws_compressor_state_t* compressor_state_ptr;

  result = lzws_compressor_get_initial_state(&compressor_state_ptr, MAX_CODE_BIT_LENGTH, BLOCK_MODE, MSB, UNALIGNED_BIT_GROUPS, QUIET);
  if (result != 0) {
    LZWS_LOG_ERROR("compressor get initial state failed");

    free(compressor_buffer);

    return 3;
  }

  char*  remaining_text        = (char*)text;
  size_t remaining_text_length = text_length;

  result = lzws_compress(compressor_state_ptr, (uint8_t**)&remaining_text, &remaining_text_length, &remaining_compressor_buffer, &remaining_compressor_buffer_length);
  if (result != LZWS_COMPRESSOR_NEEDS_MORE_SOURCE) {
    LZWS_LOG_ERROR("compressor failed");

    lzws_compressor_free_state(compressor_state_ptr);
    free(compressor_buffer);

    return 4;
  }

  result = lzws_finish_compressor(compressor_state_ptr, &remaining_compressor_buffer, &remaining_compressor_buffer_length);
  if (result != 0) {
    LZWS_LOG_ERROR("finish compressor failed");

    lzws_compressor_free_state(compressor_state_ptr);
    free(compressor_buffer);

    return 5;
  }

  lzws_compressor_free_state(compressor_state_ptr);

  // decompress

  lzws_decompressor_state_t* decompressor_state_ptr;

  result = lzws_decompressor_get_initial_state(
    &decompressor_state_ptr,
    MSB, UNALIGNED_BIT_GROUPS, QUIET);
  if (result != 0) {
    LZWS_LOG_ERROR("decompressor get initial state failed");

    free(compressor_buffer);

    return 6;
  }

  char*  remaining_compressed_text        = (char*)compressor_buffer;
  size_t remaining_compressed_text_length = compressor_buffer_length - remaining_compressor_buffer_length;

  result = lzws_decompressor_read_magic_header(decompressor_state_ptr, (uint8_t**)&remaining_compressed_text, &remaining_compressed_text_length);
  if (result != 0) {
    LZWS_LOG_ERROR("decompressor read magic header failed");

    lzws_decompressor_free_state(decompressor_state_ptr);
    free(compressor_buffer);

    return 7;
  }

  uint8_t* decompressor_buffer;
  size_t   decompressor_buffer_length = 0;

  result = lzws_create_buffer_for_decompressor(&decompressor_buffer, &decompressor_buffer_length, QUIET);
  if (result != 0) {
    LZWS_LOG_ERROR("create buffer for decompressor failed");

    lzws_decompressor_free_state(decompressor_state_ptr);
    free(compressor_buffer);

    return 8;
  }

  uint8_t* remaining_decompressor_buffer        = decompressor_buffer;
  size_t   remaining_decompressor_buffer_length = decompressor_buffer_length;

  result = lzws_decompress(
    decompressor_state_ptr,
    (uint8_t**)&remaining_compressed_text, &remaining_compressed_text_length,
    &remaining_decompressor_buffer, &remaining_decompressor_buffer_length);

  if (result != LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE) {
    LZWS_LOG_ERROR("decompressor failed");

    lzws_decompressor_free_state(decompressor_state_ptr);
    free(decompressor_buffer);
    free(compressor_buffer);

    return 9;
  }

  lzws_decompressor_free_state(decompressor_state_ptr);

  char*  decompressed_text        = (char*)decompressor_buffer;
  size_t decompressed_text_length = decompressor_buffer_length - remaining_decompressor_buffer_length;

  if (decompressed_text_length != text_length || strncmp(decompressed_text, text, text_length) != 0) {
    LZWS_LOG_ERROR("decompressed text is not the same as original");

    free(decompressor_buffer);
    free(compressor_buffer);

    return 10;
  }

  free(decompressor_buffer);
  free(compressor_buffer);

  return 0;
}
