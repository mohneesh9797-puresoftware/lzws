// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_UTILS_H)
#define LZWS_COMPRESSOR_UTILS_H

#include "../utils.h"

#include "ratio/main.h"

#include "state.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_UTILS_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_read_byte(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t* byte) {
  lzws_read_byte(source, source_length, byte);
  lzws_compressor_add_source_symbol_to_ratio(state);
}

LZWS_INLINE void lzws_compressor_write_byte(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length, uint8_t byte) {
  lzws_write_byte(destination, destination_length, byte);
  lzws_compressor_add_destination_symbol_to_ratio(state);
}

#endif // LZWS_COMPRESSOR_UTILS_H
