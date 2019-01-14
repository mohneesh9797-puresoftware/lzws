// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_WRITE_H)
#define LZWS_DECOMPRESSOR_WRITE_H

#include "state.h"

lzws_result_t lzws_decompressor_write_first_symbol(lzws_decompressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr);
lzws_result_t lzws_decompressor_write_dictionary(lzws_decompressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr);

#endif // LZWS_DECOMPRESSOR_WRITE_H
