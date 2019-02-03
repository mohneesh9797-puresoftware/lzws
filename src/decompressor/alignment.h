// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_ALIGNMENT_H)
#define LZWS_DECOMPRESSOR_ALIGNMENT_H

#include "state.h"

lzws_result_t lzws_decompressor_read_padding_zeroes_for_alignment(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr);

#endif // LZWS_DECOMPRESSOR_ALIGNMENT_H
