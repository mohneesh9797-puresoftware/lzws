// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_MAIN_H)
#define LZWS_COMPRESSOR_MAIN_H

#include "state.h"

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length);

// Use this function when you have no source (received EOF for example).
lzws_result_t lzws_flush_compressor(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length);

#endif // LZWS_COMPRESSOR_MAIN_H
