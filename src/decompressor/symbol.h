// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_SYMBOL_H)
#define LZWS_DECOMPRESSOR_SYMBOL_H

#include "state.h"

lzws_result_t lzws_decompressor_write_first_symbol(lzws_decompressor_state_t* state_ptr, lzws_symbol_t** destination_ptr, size_t* destination_length_ptr);
lzws_result_t lzws_decompressor_write_symbols_for_current_code(lzws_decompressor_state_t* state_ptr, lzws_symbol_t** destination_ptr, size_t* destination_length_ptr);

#endif // LZWS_DECOMPRESSOR_SYMBOL_H
