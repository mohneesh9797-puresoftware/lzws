// LZW streaming compressor based on well documented LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#include <stdint.h>
#include <stdlib.h>

#include "compressor.h"

// -- state --

enum {
  INITIALIZE_REFERENCES = 1,
  WRITE_MAX_SYMBOL_SIZE
};
typedef uint8_t status_t;

struct lzw_compressor_state_t {
  uint16_t max_symbol_size;

  uint8_t* first_references;
  uint8_t* next_references;
  uint8_t* terminators;

  status_t status;
};

lzw_result_t lzw_get_initial_compressor_state(lzw_compressor_state_t** result, uint8_t max_symbol_size) {
  if (max_symbol_size < LZW_LOWEST_MAX_SYMBOL_SIZE || max_symbol_size > LZW_BIGGEST_MAX_SYMBOL_SIZE) {
    return LZW_COMPRESSOR_INVALID_MAX_SYMBOL_SIZE;
  }

  lzw_compressor_state_t* state = malloc(sizeof(lzw_compressor_state_t));
  if (state == NULL) {
    return LZW_COMPRESSOR_ALLOC_FAILED;
  }

  state->max_symbol_size = max_symbol_size;

  state->first_references = NULL;
  state->next_references  = NULL;
  state->terminators      = NULL;

  state->status = INITIALIZE_REFERENCES;

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

static lzw_result_t initialize_references(lzw_compressor_state_t* state) {
  uint16_t total_codes = 1 << state->max_symbol_size;

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

  state->status = WRITE_MAX_SYMBOL_SIZE;

  return 0;
}

static lzw_result_t write_max_symbol_size(lzw_compressor_state_t* state, uint8_t** dst, size_t* dst_length) {
  if (*dst_length == 0) {
    return LZW_COMPRESSOR_NEEDS_MORE_OUTPUT;
  }

  **dst = state->max_symbol_size - 9;
  (**dst)++;
  (*dst_length)--;

  return 0;
}

lzw_result_t lzw_compress(lzw_compressor_state_t* state, const uint8_t* src, size_t src_length, uint8_t* dst, size_t* dst_length) {
  lzw_result_t result;

  if (state->status == INITIALIZE_REFERENCES) {
    result = initialize_references(state);
    if (result != 0) return result;
  }

  if (state->status == WRITE_MAX_SYMBOL_SIZE) {
    result = write_max_symbol_size(state, &dst, dst_length);
    if (result != 0) return result;
  }

  return 0;
}
