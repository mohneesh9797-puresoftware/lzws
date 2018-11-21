// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_COMPRESSOR_MAIN_H_
#define LZWS_COMPRESSOR_MAIN_H_

#include <stdint.h>
#include <stdlib.h>

#include "state.h"

// This function is optional.
// It is possible to write magic header.
lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length);

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length);

// This function is mandatory.
// Use it when you have no input (received EOF for example).
lzws_result_t lzws_compressor_flush(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length);

#endif // LZWS_COMPRESSOR_MAIN_H_
