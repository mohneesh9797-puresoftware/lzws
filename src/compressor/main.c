// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/state.h"

#include "common.h"
#include "header.h"
#include "main.h"
#include "process_code.h"
#include "read_symbol.h"
#include "write.h"

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result;

  while (true) {
    switch (state->status) {
      case LZWS_COMPRESSOR_WRITE_HEADER:
        result = lzws_compressor_write_header(state, destination, destination_length);
        break;
      case LZWS_COMPRESSOR_ALLOCATE_DICTIONARY:
        result = lzws_compressor_allocate_dictionary_in_state(state);
        break;
      case LZWS_COMPRESSOR_READ_FIRST_SYMBOL:
        result = lzws_compressor_read_first_symbol(state, source, source_length);
        break;
      case LZWS_COMPRESSOR_READ_NEXT_SYMBOL:
        result = lzws_compressor_read_next_symbol(state, source, source_length);
        break;
      case LZWS_COMPRESSOR_PROCESS_CURRENT_CODE:
        result = lzws_compressor_process_current_code(state, destination, destination_length);
        break;
      default:
        return LZWS_COMPRESSOR_UNKNOWN_STATUS;
    }

    if (result == LZWS_COMPRESSOR_NEEDS_MORE_SOURCE) {
      // Algorithm wants more source, we have finished.
      return 0;
    } else if (result != 0) {
      return result;
    }
  }
}

static inline lzws_result_t write_current_code_and_remainder(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result = lzws_compressor_write_current_code(state, destination, destination_length);
  if (result != 0) {
    return result;
  }

  return lzws_compressor_write_remainder(state, destination, destination_length);
}

lzws_result_t lzws_flush_compressor(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  switch (state->status) {
    case LZWS_COMPRESSOR_WRITE_HEADER:
    case LZWS_COMPRESSOR_ALLOCATE_DICTIONARY:
    case LZWS_COMPRESSOR_READ_FIRST_SYMBOL:
      // We have no current code and remainder yet.
      return 0;

    case LZWS_COMPRESSOR_READ_NEXT_SYMBOL:
    case LZWS_COMPRESSOR_PROCESS_CURRENT_CODE:
      // We have current code and maybe remainder.
      return write_current_code_and_remainder(state, destination, destination_length);

    default:
      return LZWS_COMPRESSOR_UNKNOWN_STATUS;
  }
}
