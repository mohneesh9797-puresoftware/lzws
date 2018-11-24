// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_STATE_H)
#define LZWS_COMPRESSOR_STATE_H

#include "../config.h"

#if defined(LZWS_DICTIONARY_TRIE_ON_LINKED_LIST)
#include "dictionary/trie-on-linked-list/main.h"
#elif defined(LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY)
#include "dictionary/trie-on-sparse-array/main.h"
#endif

enum {
  LZWS_COMPRESSOR_WRITE_HEADER = 1,
  LZWS_COMPRESSOR_ALLOCATE_DICTIONARY,
  LZWS_COMPRESSOR_READ_FIRST_SYMBOL,
  LZWS_COMPRESSOR_READ_NEXT_SYMBOL,
  LZWS_COMPRESSOR_PROCESS_CURRENT_CODE
};
typedef uint8_t lzws_compressor_status_t;

typedef struct {
  lzws_compressor_status_t status;

  uint8_t max_code_bits;
  bool    block_mode;

  lzws_code_t initial_used_code;
  lzws_code_t max_code;

  lzws_code_t last_used_code;
  uint8_t     last_used_code_bits;

  lzws_code_t current_code;
  uint8_t     next_symbol;

  uint8_t remainder;
  uint8_t remainder_bits;

  lzws_compressor_dictionary_t dictionary;
} lzws_compressor_state_t;

lzws_result_t lzws_compressor_get_initial_state(lzws_compressor_state_t** state, uint8_t max_code_bits, bool block_mode);
void          lzws_compressor_free_state(lzws_compressor_state_t* state);

#endif // LZWS_COMPRESSOR_STATE_H
