// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_COMPRESSOR_DICTIONARY_COMMON_H_
#define LZWS_COMPRESSOR_DICTIONARY_COMMON_H_

#include <stdint.h>

#include "../../common.h"

void          lzws_compressor_dictionary_initialize(void* dictionary, lzws_code_t initial_used_code);
lzws_result_t lzws_compressor_dictionary_allocate(void* dictionary, uint8_t max_code_bits);
lzws_code_t   lzws_compressor_dictionary_get_next_code(void* dictionary, lzws_code_t current_code, uint8_t symbol);
void          lzws_compressor_dictionary_save_next_code(void* dictionary, lzws_code_t current_code, lzws_code_t code, uint8_t symbol);
void          lzws_compressor_dictionary_free(void* dictionary);

#endif // LZWS_COMPRESSOR_DICTIONARY_COMMON_H_
