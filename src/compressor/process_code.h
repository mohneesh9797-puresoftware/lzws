// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_PROCESS_CODE_H)
#define LZWS_COMPRESSOR_PROCESS_CODE_H

#include "state.h"

lzws_result_t lzws_compressor_process_current_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length);

#endif // LZWS_COMPRESSOR_PROCESS_CODE_H
