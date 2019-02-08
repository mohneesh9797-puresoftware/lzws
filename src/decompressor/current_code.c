// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"

#include "../log.h"
#include "../utils.h"

#include "code.h"
#include "common.h"
#include "current_code.h"
#include "remainder.h"

lzws_result_t lzws_decompressor_read_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_code_fast_t code;

  lzws_result_t result = lzws_decompressor_read_code(state_ptr, &code, source_ptr, source_length_ptr);
  if (result != 0) {
    return result;
  }

  // It is not possible to receive clear code as first code.
  // So we need to compare first code with alphabet length.
  if (code >= LZWS_ALPHABET_LENGTH) {
    if (!state_ptr->quiet) {
      LZWS_LOG_ERROR("received invalid first code: " FAST_CODE_FORMAT, code)
    }

    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }

  state_ptr->prefix_code = code;
  state_ptr->status      = LZWS_DECOMPRESSOR_WRITE_FIRST_SYMBOL;

  return 0;
}

static inline lzws_code_fast_t get_next_code(lzws_decompressor_state_t* state_ptr)
{
  lzws_code_fast_t next_code = state_ptr->free_code;

  if (next_code == state_ptr->max_code) {
    state_ptr->free_code = LZWS_UNDEFINED_FREE_CODE;
  }
  else {
    if (next_code == state_ptr->max_free_code_for_bit_length) {
      uint_fast8_t free_code_bit_length       = ++state_ptr->free_code_bit_length;
      state_ptr->max_free_code_for_bit_length = lzws_get_mask_for_last_bits(free_code_bit_length);
    }

    state_ptr->free_code++;
  }

  return next_code;
}

lzws_result_t lzws_decompressor_read_next_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_code_fast_t code;

  lzws_result_t result = lzws_decompressor_read_code(state_ptr, &code, source_ptr, source_length_ptr);
  if (result != 0) {
    return result;
  }

  bool is_dictionary_full = lzws_decompressor_is_dictionary_full(state_ptr);
  bool quiet              = state_ptr->quiet;

  if (state_ptr->block_mode && code == LZWS_CLEAR_CODE) {
    if (!is_dictionary_full) {
      if (!quiet) {
        LZWS_LOG_ERROR("received clear code when dictionary is not full")
      }

      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }

    // We need to clear state after reading clear code.
    lzws_decompressor_clear_state(state_ptr);

    // It is possible to keep prefix code in state as is.
    // Algorithm won't touch it without reinitialization.

    if (state_ptr->unaligned_bit_groups) {
      state_ptr->status = LZWS_DECOMPRESSOR_READ_FIRST_CODE;

      return 0;
    }

    // We need to read alignment after reading clear code.
    return lzws_decompressor_verify_zero_remainder_before_read_first_code(state_ptr);
  }

  if (is_dictionary_full) {
    lzws_decompressor_write_code_to_dictionary_wrapper(state_ptr, code);
  }
  else {
    lzws_code_fast_t next_code = get_next_code(state_ptr);
    if (code > next_code) {
      if (!quiet) {
        LZWS_LOG_ERROR("received code greater than next code: " FAST_CODE_FORMAT, code)
      }

      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }

    lzws_decompressor_add_code_to_dictionary_wrapper(state_ptr, state_ptr->prefix_code, code, next_code);
  }

  state_ptr->prefix_code = code;
  state_ptr->status      = LZWS_DECOMPRESSOR_WRITE_SYMBOLS_FOR_CURRENT_CODE;

  return 0;
}
