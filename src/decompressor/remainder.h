// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_REMAINDER_H)
#define LZWS_DECOMPRESSOR_REMAINDER_H

#include "state.h"

lzws_result_t lzws_decompressor_verify_zero_remainder(lzws_decompressor_state_t* state_ptr);
lzws_result_t lzws_decompressor_verify_zero_remainder_before_read_first_code(lzws_decompressor_state_t* state_ptr);
lzws_result_t lzws_decompressor_verify_zero_remainder_before_read_next_code(lzws_decompressor_state_t* state_ptr);

#endif // LZWS_DECOMPRESSOR_REMAINDER_H
