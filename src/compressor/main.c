// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"

#include "../log.h"

#include "common.h"
#include "header.h"
#include "main.h"
#include "process_code.h"
#include "read_symbol.h"
#include "write.h"

lzws_result_t lzws_compress(lzws_compressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result;

  if (state_ptr->status == LZWS_COMPRESSOR_WRITE_HEADER) {
    result = lzws_compressor_write_header(state_ptr, destination_ptr, destination_length_ptr);
    if (result != 0) {
      return result;
    }
  }

  if (state_ptr->status == LZWS_COMPRESSOR_ALLOCATE_DICTIONARY) {
    result = lzws_compressor_allocate_dictionary_wrapper(state_ptr);
    if (result != 0) {
      return result;
    }
  }

  if (state_ptr->status == LZWS_COMPRESSOR_READ_FIRST_SYMBOL) {
    result = lzws_compressor_read_first_symbol(state_ptr, source_ptr, source_length_ptr);
    if (result != 0) {
      return result;
    }
  }

  while (true) {
    lzws_compressor_status_t status = state_ptr->status;

    switch (status) {
      case LZWS_COMPRESSOR_READ_NEXT_SYMBOL:
        result = lzws_compressor_read_next_symbol(state_ptr, source_ptr, source_length_ptr);
        break;

      case LZWS_COMPRESSOR_PROCESS_CURRENT_CODE:
        result = lzws_compressor_process_current_code(state_ptr, destination_ptr, destination_length_ptr);
        break;

      case LZWS_COMPRESSOR_WRITE_DESTINATION_REMAINDER_FOR_ALIGNMENT:
        result = lzws_compressor_write_destination_remainder_for_alignment(state_ptr, destination_ptr, destination_length_ptr);
        break;

      case LZWS_COMPRESSOR_WRITE_ALIGNMENT:
        result = lzws_compressor_write_alignment(state_ptr, destination_ptr, destination_length_ptr);
        break;

      default:
        if (!state_ptr->quiet) {
          LZWS_PRINTF_ERROR("unknown status: %u", status)
        }

        return LZWS_COMPRESSOR_UNKNOWN_STATUS;
    }

    if (result != 0) {
      return result;
    }
  }
}

lzws_result_t lzws_flush_compressor(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr) {
  lzws_result_t result;

  switch (state_ptr->status) {
    case LZWS_COMPRESSOR_READ_FIRST_SYMBOL:
      // We have no current code and destination remainder yet.
      return 0;

    case LZWS_COMPRESSOR_READ_NEXT_SYMBOL:
      // We have current code and maybe destination remainder.
      result = lzws_compressor_flush_current_code(state_ptr, destination_ptr, destination_length_ptr);
      if (result != 0) {
        return result;
      }
      break;
  }

  lzws_compressor_status_t status = state_ptr->status;

  switch (status) {
    case LZWS_COMPRESSOR_WRITE_DESTINATION_REMAINDER:
      return lzws_compressor_write_destination_remainder(state_ptr, destination_ptr, destination_length_ptr);

    default:
      if (!state_ptr->quiet) {
        LZWS_PRINTF_ERROR("unknown status: %u", status)
      }

      return LZWS_COMPRESSOR_UNKNOWN_STATUS;
  }
}
