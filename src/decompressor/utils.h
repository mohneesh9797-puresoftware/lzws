// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_UTILS_H)
#define LZWS_DECOMPRESSOR_UTILS_H

#include "state.h"

void lzws_decompressor_read_byte(lzws_decompressor_state_t* state, lzws_symbol_fast_t* byte_ptr, lzws_symbol_t** source_ptr, size_t* source_length_ptr);

#endif // LZWS_DECOMPRESSOR_UTILS_H
