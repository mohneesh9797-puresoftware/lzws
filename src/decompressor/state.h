// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_STATE_H)
#define LZWS_DECOMPRESSOR_STATE_H

#include "dictionary/common.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_STATE_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

enum {
  LZWS_DECOMPRESSOR_READ_HEADER = 1,
  LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY,
  LZWS_DECOMPRESSOR_READ_FIRST_CODE,
  LZWS_DECOMPRESSOR_READ_NEXT_CODE,
  LZWS_DECOMPRESSOR_WRITE_FIRST_SYMBOL,
  LZWS_DECOMPRESSOR_WRITE_SYMBOLS_FROM_DICTIONARY,
  LZWS_DECOMPRESSOR_VERIFY_EMPTY_SOURCE_REMAINDER_FOR_ALIGNMENT,
  LZWS_DECOMPRESSOR_READ_PADDING_ZEROES_FOR_ALIGNMENT
};
typedef uint_fast8_t lzws_decompressor_status_t;

typedef struct {
  lzws_decompressor_status_t status;

  bool block_mode;
  bool msb;
  bool quiet;
  bool unaligned_bit_groups;

  lzws_code_fast_t initial_used_code;
  lzws_code_fast_t max_code;

  lzws_code_fast_t last_used_code;
  lzws_code_fast_t last_used_max_code;
  uint_fast8_t     last_used_code_bit_length;

  lzws_code_fast_t prefix_code;

  uint_fast8_t source_remainder;
  uint_fast8_t source_remainder_bit_length;

  uint_fast8_t unaligned_by_code_bit_length;
  uint_fast8_t unaligned_source_byte_length;

  lzws_decompressor_dictionary_t dictionary;
} lzws_decompressor_state_t;

lzws_result_t lzws_decompressor_get_initial_state(lzws_decompressor_state_t** state_ptr, bool msb, bool quiet, bool unaligned_bit_groups);
void          lzws_decompressor_clear_state(lzws_decompressor_state_t* state_ptr);
void          lzws_decompressor_free_state(lzws_decompressor_state_t* state_ptr);

LZWS_INLINE bool lzws_decompressor_is_dictionary_full(lzws_decompressor_state_t* state_ptr)
{
  return state_ptr->last_used_code == state_ptr->max_code;
}

// Source byte length can be aligned by unaligned by code bit length * 8.

LZWS_INLINE void lzws_compressor_update_unaligned_source_byte_length(lzws_decompressor_state_t* state_ptr, uint_fast8_t source_byte_length)
{
  state_ptr->unaligned_source_byte_length = (state_ptr->unaligned_source_byte_length + source_byte_length) % state_ptr->unaligned_by_code_bit_length;
}

#endif // LZWS_DECOMPRESSOR_STATE_H
