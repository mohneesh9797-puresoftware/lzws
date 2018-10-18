// LZW streaming compressor based on LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "compressor.h"

// -- state --

enum {
  INITIALIZE = 1,
  WRITE_HEADER,
  GET_PREFIX,
  PROCESS_SYMBOL,
  FINISHED
};
typedef uint8_t status_t;

struct lzw_compressor_state_t {
  uint16_t max_bits;
  bool     block_mode;

  uint8_t* first_references;
  uint8_t* next_references;
  uint8_t* terminators;

  uint8_t prefix;

  status_t status;
};

lzw_result_t lzw_get_initial_compressor_state(lzw_compressor_state_t** result, uint8_t max_bits, bool block_mode) {
  if (max_bits < LZW_LOWEST_MAX_BITS || max_bits > LZW_BIGGEST_MAX_BITS) {
    return LZW_COMPRESSOR_INVALID_MAX_BITS;
  }

  lzw_compressor_state_t* state = malloc(sizeof(lzw_compressor_state_t));
  if (state == NULL) {
    return LZW_COMPRESSOR_ALLOC_FAILED;
  }

  state->max_bits   = max_bits;
  state->block_mode = block_mode;

  state->first_references = NULL;
  state->next_references  = NULL;
  state->terminators      = NULL;

  state->status = WRITE_HEADER;

  *result = state;

  return 0;
}

void lzw_free_compressor_state(lzw_compressor_state_t* state) {
  uint8_t* first_references = state->first_references;
  if (first_references != NULL) free(first_references);

  uint8_t* next_references = state->next_references;
  if (next_references != NULL) free(next_references);

  uint8_t* terminators = state->terminators;
  if (terminators != NULL) free(terminators);

  free(state);
}

// -- compress --

// 2 bytes for magic header (for lzw autodetection) and 1 byte for "block_mode" and "max_bits".

static lzw_result_t write_header(lzw_compressor_state_t* state, uint8_t** dst, size_t* dst_length) {
  uint8_t data = state->max_bits;
  if (state->block_mode) {
    data = data | LZW_BLOCK_MODE;
  }

  uint8_t magic_header_size = sizeof(LZW_MAGIC_HEADER);
  uint8_t data_size         = sizeof(data);
  uint8_t header_size       = magic_header_size + data_size;

  if (*dst_length < header_size) {
    return LZW_COMPRESSOR_NEEDS_MORE_DST;
  }

  memcpy(*dst, &LZW_MAGIC_HEADER, magic_header_size);
  (**dst) += magic_header_size;

  memcpy(*dst, &data, data_size);
  (**dst) += data_size;

  (*dst_length) -= header_size;

  state->status = INITIALIZE;

  return 0;
}

static lzw_result_t initialize(lzw_compressor_state_t* state) {
  uint16_t total_codes = 1 << state->max_bits;

  uint8_t* first_references = calloc(1, total_codes * sizeof(uint8_t));
  uint8_t* next_references  = calloc(1, (total_codes - 256) * sizeof(uint8_t));
  uint8_t* terminators      = calloc(1, (total_codes - 256) * sizeof(uint8_t));

  if (first_references == NULL || next_references == NULL || terminators == NULL) {
    if (first_references != NULL) free(first_references);
    if (next_references != NULL) free(next_references);
    if (terminators != NULL) free(terminators);

    return LZW_COMPRESSOR_ALLOC_FAILED;
  }

  state->first_references = first_references;
  state->next_references  = next_references;
  state->terminators      = terminators;

  state->status = GET_PREFIX;

  return 0;
}

static lzw_result_t get_symbol(uint8_t** src, size_t* src_length, uint8_t* symbol) {
  if (*src_length < 1) {
    return LZW_COMPRESSOR_NEEDS_MORE_SRC;
  }

  *symbol = **src;
  (**src)++;
  (*src_length)--;

  return 0;
}

static lzw_result_t get_prefix(lzw_compressor_state_t* state, uint8_t** src, size_t* src_length) {
  lzw_result_t result = get_symbol(src, src_length, &state->prefix);
  if (result != 0) return result;

  state->status = PROCESS_SYMBOL;

  return 0;
}

static lzw_result_t process_symbol(lzw_compressor_state_t* state, uint8_t** src, size_t* src_length, uint8_t** dst, size_t* dst_length) {
  uint8_t      symbol;
  lzw_result_t result = get_symbol(src, src_length, &symbol);
  if (result != 0) return result;

  // If any longer strings are built on the current prefix.
  uint8_t index = state->first_references[state->prefix];

  return 0;
}

lzw_result_t lzw_compress(lzw_compressor_state_t* state, uint8_t* src, size_t* src_length, uint8_t* dst, size_t* dst_length) {
  lzw_result_t result;

  while (true) {
    switch (state->status) {
      case WRITE_HEADER:
        result = write_header(state, &dst, dst_length);
        break;
      case INITIALIZE:
        result = initialize(state);
        break;
      case GET_PREFIX:
        result = get_prefix(state, &src, src_length);
        break;
      case PROCESS_SYMBOL:
        result = process_symbol(state, &src, src_length, &dst, dst_length);
        break;
      case FINISHED:
        return 0;
      default:
        return LZW_COMPRESSOR_UNKNOWN_STATUS;
    }

    if (result != 0) return result;
  }
}
