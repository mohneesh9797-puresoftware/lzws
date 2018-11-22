// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <stdint.h>
#include <stdlib.h>

#include "decompressor.h"

// -- state --

enum {
  READ_MAXBITS = 1
};
typedef uint8_t status_t;

struct lzws_decompressor_state_t {
  status_t status;
};

lzws_result_t lzws_get_initial_decompressor_state(lzws_decompressor_state_t** result) {
  lzws_decompressor_state_t* state = malloc(sizeof(lzws_decompressor_state_t));
  if (state == NULL) {
    return LZWS_DECOMPRESSOR_ALLOC_FAILED;
  }

  state->status = READ_MAXBITS;

  *result = state;
  return 0;
}

void lzws_free_decompressor_state(lzws_decompressor_state_t* state) {
  free(state);
}

// -- decompress --

static lzws_result_t read_byte(uint8_t* result, uint8_t** src, size_t* src_length) {
  if (*src_length == 0) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_INPUT;
  }

  *result = **src;
  (*src)++;
  (*src_length)--;

  return 0;
}

static lzws_result_t read_max_bits(uint8_t** src, size_t* src_length) {
  uint8_t byte;

  lzws_result_t result = read_byte(&byte, src, src_length);
  if (result) {
    return result;
  }

  if (byte > MAX_MAXBITS - MIN_MAXBITS) {
    ;
  }

  return 0;
}

lzws_result_t lzws_decompress(lzws_decompressor_state_t* state,
                              const uint8_t* src, size_t src_length,
                              uint8_t* dst, size_t* dst_length) {
  lzws_result_t result;
  uint8_t**     src_ptr = (uint8_t**)&src;
  status_t      status  = state->status;

  if (status == READ_MAXBITS) {
    result = read_max_bits(src_ptr, &src_length);
    if (result) {
      return result;
    }
  }

  return 0;
}
