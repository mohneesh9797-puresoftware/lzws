// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../../decompressor/header.h"
#include "../../decompressor/common.h"
#include "../../log.h"
#include "../../macro.h"
#include "../combination.h"

#include "header.h"

#define MAGIC_HEADER_SIZE 2
static const uint8_t magic_headers[][MAGIC_HEADER_SIZE] = {
  {LZWS_FIRST_MAGIC_HEADER_BYTE + 1, LZWS_SECOND_MAGIC_HEADER_BYTE}, // First byte is invalid.
  {LZWS_FIRST_MAGIC_HEADER_BYTE, LZWS_SECOND_MAGIC_HEADER_BYTE + 1}  // Second byte is invalid.
};
#define MAGIC_HEADER_LENGTH sizeof(magic_headers) / MAGIC_HEADER_SIZE

#define HEADER_SIZE 1
static const uint8_t headers[][HEADER_SIZE] = {
  {(LZWS_LOWEST_MAX_CODE_BIT_LENGTH - 1) | LZWS_BLOCK_MODE}};
#define HEADER_LENGTH sizeof(headers) / HEADER_SIZE

static inline lzws_result_t test_invalid_header(lzws_decompressor_state_t* state_ptr, size_t LZWS_UNUSED(buffer_length), va_list LZWS_UNUSED(args))
{
  size_t index;

  for (index = 0; index < MAGIC_HEADER_LENGTH; index++) {
    uint8_t* magic_header      = (uint8_t*)magic_headers[index];
    size_t   magic_header_size = MAGIC_HEADER_SIZE;

    if (lzws_decompressor_read_magic_header(state_ptr, &magic_header, &magic_header_size) != LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER) {
      LZWS_LOG_ERROR("decompressor read magic header should fail with invalid magic header");
      return 1;
    }
  }

  for (index = 0; index < HEADER_LENGTH; index++) {
    uint8_t* header      = (uint8_t*)headers[index];
    size_t   header_size = HEADER_SIZE;

    if (lzws_decompressor_read_header(state_ptr, &header, &header_size) != LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH) {
      LZWS_LOG_ERROR("decompressor read header should fail with invalid max code bit length");
      return 2;
    }
  }

  return 0;
}

lzws_result_t lzws_test_invalid_header()
{
  return lzws_test_decompressor_combinations(test_invalid_header);
}
