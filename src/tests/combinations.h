// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_TESTS_COMBINATIONS_H)
#define LZWS_TESTS_COMBINATIONS_H

#include <stdarg.h>

#include "../compressor/state.h"
#include "../decompressor/state.h"

typedef lzws_result_t (*lzws_test_compressor_t)(lzws_compressor_state_t* state_ptr, va_list args);
typedef lzws_result_t (*lzws_test_decompressor_t)(lzws_decompressor_state_t* state_ptr, va_list args);

lzws_result_t lzws_test_compressor_combinations(lzws_test_compressor_t* function_ptr, ...);
lzws_result_t lzws_test_decompressor_combinations(lzws_test_decompressor_t* function_ptr, ...);

#endif // LZWS_TESTS_COMBINATIONS_H
