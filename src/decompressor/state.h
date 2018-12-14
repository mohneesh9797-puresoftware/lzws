// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_STATE_H)
#define LZWS_DECOMPRESSOR_STATE_H

#include "dictionary/type.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_STATE_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

enum {
  LZWS_DECOMPRESSOR_READ_HEADER = 1,
  LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY
};
typedef uint_fast8_t lzws_decompressor_status_t;

typedef struct {
  lzws_decompressor_status_t status;

  uint_fast8_t max_code_bits;
  bool         block_mode;
  bool         msb;

  lzws_code_fast_t initial_used_code;

  lzws_decompressor_dictionary_t dictionary;
} lzws_decompressor_state_t;

lzws_result_t lzws_decompressor_get_initial_state(lzws_decompressor_state_t** state_ptr, bool msb);
void          lzws_decompressor_free_state(lzws_decompressor_state_t* state_ptr);

#endif // LZWS_DECOMPRESSOR_STATE_H
