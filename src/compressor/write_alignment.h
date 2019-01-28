// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_WRITE_ALIGNMENT_H)
#define LZWS_COMPRESSOR_WRITE_ALIGNMENT_H

#include "state.h"

lzws_result_t lzws_compressor_write_padding_zeroes_for_alignment(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr);

#endif // LZWS_COMPRESSOR_WRITE_ALIGNMENT_H
