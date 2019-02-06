// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_STATE_H)
#define LZWS_COMPRESSOR_STATE_H

#include "alignment/common.h"
#include "dictionary/common.h"
#include "ratio/common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_STATE_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

enum {
  LZWS_COMPRESSOR_WRITE_HEADER = 1,
  LZWS_COMPRESSOR_ALLOCATE_DICTIONARY,
  LZWS_COMPRESSOR_READ_FIRST_SYMBOL,
  LZWS_COMPRESSOR_READ_NEXT_SYMBOL,
  LZWS_COMPRESSOR_WRITE_CURRENT_CODE,
  LZWS_COMPRESSOR_FLUSH_CURRENT_CODE,
  LZWS_COMPRESSOR_WRITE_REMAINDER_BEFORE_READ_NEXT_SYMBOL,
  LZWS_COMPRESSOR_WRITE_REMAINDER_BEFORE_CURRENT_CODE,
  LZWS_COMPRESSOR_FLUSH_REMAINDER_BEFORE_CURRENT_CODE,
  LZWS_COMPRESSOR_FLUSH_REMAINDER,
  LZWS_COMPRESSOR_WRITE_ALIGNMENT_BEFORE_READ_NEXT_SYMBOL,
  LZWS_COMPRESSOR_WRITE_ALIGNMENT_BEFORE_CURRENT_CODE,
  LZWS_COMPRESSOR_FLUSH_ALIGNMENT_BEFORE_CURRENT_CODE
};
typedef uint_fast8_t lzws_compressor_status_t;

typedef struct {
  lzws_compressor_status_t status;

  uint_fast8_t max_code_bit_length;
  bool         block_mode;
  bool         msb;
  bool         quiet;
  bool         unaligned_bit_groups;

  lzws_code_fast_t first_free_code;
  lzws_code_fast_t max_code;

  lzws_code_fast_t last_used_code;
  lzws_code_fast_t last_used_max_code;
  uint_fast8_t     last_used_code_bit_length;

  lzws_code_fast_t current_code;
  uint_fast8_t     next_symbol;

  uint_fast8_t remainder;
  uint_fast8_t remainder_bit_length;

  lzws_compressor_alignment_t  alignment;
  lzws_compressor_dictionary_t dictionary;
  lzws_compressor_ratio_t      ratio;
} lzws_compressor_state_t;

lzws_result_t lzws_compressor_get_initial_state(
  lzws_compressor_state_t** state_ptr,
  uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool quiet, bool unaligned);

void lzws_compressor_clear_state(lzws_compressor_state_t* state_ptr);
void lzws_compressor_free_state(lzws_compressor_state_t* state_ptr);

LZWS_INLINE bool lzws_compressor_is_dictionary_full(lzws_compressor_state_t* state_ptr)
{
  return state_ptr->last_used_code == state_ptr->max_code;
}

LZWS_INLINE lzws_code_fast_t lzws_compressor_get_initial_used_code(lzws_compressor_state_t* state_ptr)
{
  return state_ptr->first_free_code - 1;
}

LZWS_INLINE size_t lzws_compressor_get_total_codes_length(lzws_compressor_state_t* state_ptr)
{
  return state_ptr->max_code + 1;
}

#endif // LZWS_COMPRESSOR_STATE_H
