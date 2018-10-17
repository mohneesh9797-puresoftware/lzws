// LZW streaming compressor based on well documented LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "compressor.h"

// -- state --

enum {
  INITIALIZE_REFERENCES = 1,
  WRITE_HEADER
};
typedef uint8_t status_t;

struct lzw_compressor_state_t {
  uint16_t max_bits;
  bool     block_mode;

  uint8_t* first_references;
  uint8_t* next_references;
  uint8_t* terminators;

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

static lzw_result_t write_header(lzw_compressor_state_t* state, uint8_t** dst, size_t* dst_length) {
  uint8_t max_bits = state->max_bits;

  uint8_t magic_header_size = sizeof(LZW_MAGIC_HEADER);
  uint8_t max_bits_size     = sizeof(max_bits);
  uint8_t header_size       = magic_header_size + max_bits_size;

  if (*dst_length < header_size) {
    return LZW_COMPRESSOR_NEEDS_MORE_DST;
  }

  memcpy(*dst, &LZW_MAGIC_HEADER, magic_header_size);
  (**dst) += magic_header_size;

  memcpy(*dst, &max_bits, max_bits_size);
  (**dst) += max_bits_size;

  (*dst_length) -= header_size;

  state->status = INITIALIZE_REFERENCES;

  return 0;
}

static lzw_result_t initialize_references(lzw_compressor_state_t* state) {
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

  // state->status =

  return 0;
}

lzw_result_t lzw_compress(lzw_compressor_state_t* state, const uint8_t* src, size_t src_length, uint8_t* dst, size_t* dst_length) {
  lzw_result_t result;

  if (state->status == WRITE_HEADER) {
    result = write_header(state, &dst, dst_length);
    if (result != 0) return result;
  }

  if (state->status == INITIALIZE_REFERENCES) {
    result = initialize_references(state);
    if (result != 0) return result;
  }

  return 0;
}
