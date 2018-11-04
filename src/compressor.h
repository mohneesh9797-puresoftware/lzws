// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_COMPRESSOR_H_
#define LZWS_COMPRESSOR_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

// Possible failed results:
enum {
  LZWS_COMPRESSOR_ALLOCATE_FAILED = 1,
  LZWS_COMPRESSOR_INVALID_MAX_BITS,
  LZWS_COMPRESSOR_NEEDS_MORE_SOURCE,
  LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION,
  LZWS_COMPRESSOR_UNKNOWN_STATUS
};

// Possible statuses:
enum {
  LZWS_COMPRESSOR_WRITE_HEADER = 1,
  LZWS_COMPRESSOR_ALLOCATE_DICTIONARY,
  LZWS_COMPRESSOR_GET_FIRST_SYMBOL,
  LZWS_COMPRESSOR_PROCESS_SYMBOL
};
typedef uint8_t lzws_status_t;

typedef struct lzws_compressor_state_t {
  lzws_status_t status;

  uint8_t max_bits;
  bool    block_mode;

  uint16_t* first_child_codes;
  uint16_t* next_sibling_codes;
  uint8_t*  symbol_by_codes;

  uint16_t current_code;
  uint16_t prev_code;
} lzws_compressor_state_t;

// It is possible to write magic header.
// This function is optional.
lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length);

lzws_result_t lzws_get_initial_compressor_state(lzws_compressor_state_t** state, uint8_t max_bits, bool block_mode);
void          lzws_free_compressor_state(lzws_compressor_state_t* state);

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length);

#endif // LZWS_COMPRESSOR_H_
