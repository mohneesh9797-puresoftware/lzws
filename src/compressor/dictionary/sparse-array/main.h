// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY_MAIN_H)
#define LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY_MAIN_H

#include <stdlib.h>

#include "common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t first_free_code, bool block_mode)
{
  dictionary_ptr->next_codes = NULL;

  dictionary_ptr->first_free_code = first_free_code;

  // We won't store clear code.
  dictionary_ptr->next_codes_offset = first_free_code - LZWS_ALPHABET_LENGTH;

  if (block_mode) {
    dictionary_ptr->used_indexes = NULL;

    // We won't store char codes and clear code.
    dictionary_ptr->used_indexes_offset = first_free_code;
  }
}

lzws_result_t lzws_compressor_allocate_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length, bool block_mode, bool quiet);
void          lzws_compressor_clear_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, size_t total_codes_length);

lzws_code_fast_t lzws_compressor_get_next_code_from_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, lzws_code_fast_t current_code, uint_fast8_t next_symbol);

void lzws_compressor_save_next_code_to_dictionary(
  lzws_compressor_dictionary_t* dictionary_ptr, bool block_mode,
  lzws_code_fast_t current_code, uint_fast8_t next_symbol, lzws_code_fast_t next_code);

LZWS_INLINE void lzws_compressor_free_dictionary(lzws_compressor_dictionary_t* dictionary_ptr, bool block_mode)
{
  lzws_code_t* next_codes = dictionary_ptr->next_codes;
  if (next_codes != NULL) {
    free(next_codes);
  }

  if (block_mode) {
    lzws_compressor_dictionary_used_index_t* used_indexes = dictionary_ptr->used_indexes;
    if (used_indexes != NULL) {
      free(used_indexes);
    }
  }
}

#endif // LZWS_COMPRESSOR_DICTIONARY_SPARSE_ARRAY_MAIN_H
