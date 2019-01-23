// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_UTILS_H)
#define LZWS_COMPRESSOR_UTILS_H

#include "state.h"

void lzws_compressor_read_byte(lzws_compressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, uint_fast8_t* byte_ptr);
void lzws_compressor_write_byte(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr, uint_fast8_t byte);

#endif // LZWS_COMPRESSOR_UTILS_H
