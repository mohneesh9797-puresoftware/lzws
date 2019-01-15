// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"

#include "header.h"
#include "main.h"
#include "read_code.h"
#include "write.h"

lzws_result_t lzws_decompress(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result;

  if (state_ptr->status == LZWS_DECOMPRESSOR_READ_HEADER) {
    result = lzws_decompressor_read_header(state_ptr, source_ptr, source_length_ptr);
    if (result != 0) {
      return result;
    }
  }

  if (state_ptr->status == LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY) {
    result = lzws_decompressor_allocate_dictionary_wrapper(state_ptr);
    if (result != 0) {
      return result;
    }
  }

  while (true) {
    switch (state_ptr->status) {
      case LZWS_DECOMPRESSOR_READ_FIRST_CODE:
        result = lzws_decompressor_read_first_code(state_ptr, source_ptr, source_length_ptr);
        break;

      case LZWS_DECOMPRESSOR_READ_NEXT_CODE:
        result = lzws_decompressor_read_next_code(state_ptr, source_ptr, source_length_ptr);
        break;

      case LZWS_DECOMPRESSOR_WRITE_FIRST_SYMBOL:
        result = lzws_decompressor_write_first_symbol(state_ptr, destination_ptr, destination_length_ptr);
        break;

      case LZWS_DECOMPRESSOR_WRITE_DICTIONARY:
        result = lzws_decompressor_write_dictionary(state_ptr, destination_ptr, destination_length_ptr);
        break;

      default:
        return LZWS_DECOMPRESSOR_UNKNOWN_STATUS;
    }

    if (result != 0) {
      return result;
    }
  }

  return 0;
}

lzws_result_t lzws_flush_decompressor(lzws_decompressor_state_t* state_ptr) {
  switch (state_ptr->status) {
    case LZWS_DECOMPRESSOR_READ_HEADER:
    case LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY:
      // We have no source remainder yet.
      return 0;

    case LZWS_DECOMPRESSOR_READ_FIRST_CODE:
    case LZWS_DECOMPRESSOR_READ_NEXT_CODE:
    case LZWS_DECOMPRESSOR_WRITE_FIRST_SYMBOL:
    case LZWS_DECOMPRESSOR_WRITE_DICTIONARY:
      // We may have source remainder.
      return lzws_decompressor_verify_empty_source_remainder(state_ptr);

    default:
      return LZWS_DECOMPRESSOR_UNKNOWN_STATUS;
  }
}
