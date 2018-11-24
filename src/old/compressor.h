// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_H)
#define LZWS_COMPRESSOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

// Possible results:
enum {
  LZWS_COMPRESSOR_ALLOCATE_FAILED = 1,
  LZWS_COMPRESSOR_INVALID_MAX_CODE_BITS,
  LZWS_COMPRESSOR_NEEDS_MORE_SOURCE,
  LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION,
  LZWS_COMPRESSOR_UNKNOWN_STATUS
};

// Possible statuses:
enum {
  LZWS_COMPRESSOR_WRITE_HEADER = 1,
  LZWS_COMPRESSOR_ALLOCATE_DICTIONARY,
  LZWS_COMPRESSOR_READ_NEXT_SYMBOL,
  LZWS_COMPRESSOR_WRITE_CURRENT_CODE
};
typedef uint8_t lzws_compressor_status_t;

typedef struct lzws_compressor_state_t {
  lzws_compressor_status_t status;

  uint8_t max_code_bits;
  bool    block_mode;

  lzws_code_t* first_child_codes;
  lzws_code_t* next_sibling_codes;
  uint8_t*     symbol_by_codes;

  lzws_code_t last_used_code;
  uint8_t     last_used_code_bits;

  lzws_code_t prefix_code;
  uint8_t     prefix_code_bits;
  lzws_code_t current_code;
  uint8_t     current_code_bits;

  uint8_t remainder;
  uint8_t remainder_bits;
} lzws_compressor_state_t;

// It is possible to write magic header.
// This function is optional.
// Use it to be compatible with original "compress" utility.
lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length);

lzws_result_t lzws_compressor_get_initial_state(lzws_compressor_state_t** state, uint8_t max_code_bits, bool block_mode);
void          lzws_compressor_free_state(lzws_compressor_state_t* state);

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length);

// This function is mandatory.
// Use it when you have no input (for example received EOF).
lzws_result_t lzws_compressor_flush(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length);

#endif // LZWS_COMPRESSOR_H
