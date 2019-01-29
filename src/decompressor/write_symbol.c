// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"

#include "../utils.h"

#include "common.h"
#include "write_symbol.h"

lzws_result_t lzws_decompressor_write_first_symbol(lzws_decompressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  if (*destination_length_ptr < 1) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_write_byte(state_ptr->prefix_code, destination_ptr, destination_length_ptr);

  state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_NEXT_CODE;

  return 0;
}

lzws_result_t lzws_decompressor_write_symbols_from_dictionary(lzws_decompressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  uint8_t symbol;

  while (lzws_decompressor_has_symbol_in_dictionary_wrapper(state_ptr)) {
    if (*destination_length_ptr < 1) {
      return LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION;
    }

    symbol = lzws_decompressor_get_symbol_from_dictionary_wrapper(state_ptr);
    lzws_write_byte(symbol, destination_ptr, destination_length_ptr);
  }

  state_ptr->status = LZWS_DECOMPRESSOR_PROCESS_NEXT_CODE;

  return 0;
}
