// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

// #include "dictionary/main.h"

#include "main.h"
#include "common.h"
#include "header.h"

lzws_result_t lzws_decompress(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result;

  if (state_ptr->status == LZWS_DECOMPRESSOR_READ_HEADER) {
    result = lzws_decompressor_read_header(state_ptr, source_ptr, source_length_ptr);

    if (result != 0) {
      return result;
    }
  }

  // if (state_ptr->status == LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY) {
  //   result = lzws_decompressor_allocate_dictionary(state_ptr);
  //
  //   if (result != 0) {
  //     return result;
  //   }
  // }

  // if (state_ptr->status == LZWS_DECOMPRESSOR_READ_FIRST_SYMBOL) {
  //   result = lzws_decompressor_read_first_symbol(state_ptr, source_ptr, source_length_ptr);
  //
  //   if (result == LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE) {
  //     // Algorithm wants more source, we have finished.
  //     return 0;
  //   } else if (result != 0) {
  //     return result;
  //   }
  // }

  // while (true) {
  //   switch (state_ptr->status) {
  //     case LZWS_DECOMPRESSOR_READ_NEXT_SYMBOL:
  //       result = lzws_decompressor_read_next_symbol(state_ptr, source_ptr, source_length_ptr);
  //
  //       if (result == LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE) {
  //         // Algorithm wants more source, we have finished.
  //         return 0;
  //       }
  //       break;
  //
  //     case LZWS_DECOMPRESSOR_PROCESS_CURRENT_CODE:
  //       result = lzws_decompressor_process_current_code(state_ptr, destination_ptr, destination_length_ptr);
  //       break;
  //
  //     default:
  //       return LZWS_DECOMPRESSOR_UNKNOWN_STATUS;
  //   }
  //
  //   if (result != 0) {
  //     return result;
  //   }
  // }

  return 0;
}
