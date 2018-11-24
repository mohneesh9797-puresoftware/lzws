// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_COMMON_H)
#define LZWS_COMPRESSOR_DICTIONARY_COMMON_H

#include "../../common.h"

void          lzws_compressor_initialize_dictionary(void* dictionary, lzws_code_t initial_used_code);
lzws_result_t lzws_compressor_allocate_dictionary(void* dictionary, uint8_t max_code_bits);
lzws_code_t   lzws_compressor_get_next_code_from_dictionary(void* dictionary, lzws_code_t current_code, uint8_t symbol);
void          lzws_compressor_save_next_code_to_dictionary(void* dictionary, lzws_code_t current_code, uint8_t symbol, lzws_code_t code);
void          lzws_compressor_free_dictionary(void* dictionary);

#endif // LZWS_COMPRESSOR_DICTIONARY_COMMON_H
