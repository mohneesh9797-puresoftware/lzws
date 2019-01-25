// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "dictionary/wrapper.h"

#include "../log.h"
#include "../utils.h"

#include "common.h"
#include "read_code.h"
#include "utils.h"

static inline void add_byte(lzws_code_fast_t* code_ptr, uint_fast8_t code_bit_length, uint_fast8_t byte, bool msb)
{
  lzws_code_fast_t code = *code_ptr;

  if (msb) {
    // Code is sitting on the top.
    code = (code << 8) | byte;
  }
  else {
    // Code is sitting on the bottom.
    code = (byte << code_bit_length) | code;
  }

  *code_ptr = code;
}

static inline void add_byte_with_remainder(
  lzws_code_fast_t* code_ptr, uint_fast8_t code_bit_length, uint_fast8_t target_code_bit_length,
  uint_fast8_t byte, uint_fast8_t* source_remainder_ptr, uint_fast8_t* source_remainder_bit_length_ptr,
  bool msb)
{
  uint_fast8_t code_part_bit_length = target_code_bit_length - code_bit_length;
  if (code_part_bit_length == 8) {
    add_byte(code_ptr, code_bit_length, byte, msb);

    *source_remainder_ptr            = 0;
    *source_remainder_bit_length_ptr = 0;

    return;
  }

  lzws_code_fast_t code = *code_ptr;
  uint_fast8_t     code_part;

  uint_fast8_t source_remainder;
  uint_fast8_t source_remainder_bit_length = 8 - code_part_bit_length;

  if (msb) {
    // Taking first bits from byte.
    code_part = byte >> source_remainder_bit_length;

    // Last bits of byte is a remainder.
    source_remainder = byte & lzws_get_mask_for_last_bits(source_remainder_bit_length);

    // Code is sitting on the top.
    code = (code << code_part_bit_length) | code_part;
  }
  else {
    // Taking last bits from byte.
    code_part = byte & lzws_get_mask_for_last_bits(code_part_bit_length);

    // First bits of byte is a remainder.
    source_remainder = byte >> code_part_bit_length;

    // Code is sitting on the bottom.
    code = (code_part << code_bit_length) | code;
  }

  *code_ptr                        = code;
  *source_remainder_ptr            = source_remainder;
  *source_remainder_bit_length_ptr = source_remainder_bit_length;
}

static inline uint_fast8_t get_target_code_bit_length(lzws_decompressor_state_t* state_ptr)
{
  uint_fast8_t last_used_code_bit_length = state_ptr->last_used_code_bit_length;

  if (lzws_decompressor_is_dictionary_full(state_ptr) || state_ptr->last_used_code != state_ptr->last_used_max_code) {
    return last_used_code_bit_length;
  }

  return last_used_code_bit_length + 1;
}

lzws_result_t lzws_decompressor_read_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, lzws_code_fast_t* code_ptr)
{
  uint_fast8_t target_code_bit_length      = get_target_code_bit_length(state_ptr);
  uint_fast8_t source_remainder_bit_length = state_ptr->source_remainder_bit_length;

  // Target code bit length will always be >= 8.
  // Source remainder bit length will always be <= 7.
  // So source byte length will always be >= 1.

  uint_fast8_t source_byte_length = lzws_ceil_bit_length_to_byte_length(target_code_bit_length - source_remainder_bit_length);
  if (*source_length_ptr < source_byte_length) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  lzws_code_fast_t code            = state_ptr->source_remainder;
  uint_fast8_t     code_bit_length = source_remainder_bit_length;
  bool             msb             = state_ptr->msb;

  uint_fast8_t byte;

  while (source_byte_length != 1) {
    lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);
    add_byte(&code, code_bit_length, byte, msb);

    code_bit_length += 8;
    source_byte_length--;
  }

  lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);
  add_byte_with_remainder(
    &code, code_bit_length, target_code_bit_length,
    byte, &state_ptr->source_remainder, &state_ptr->source_remainder_bit_length,
    msb);

  *code_ptr = code;

  return 0;
}

lzws_result_t lzws_decompressor_read_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_code_fast_t code;

  lzws_result_t result = lzws_decompressor_read_code(state_ptr, source_ptr, source_length_ptr, &code);
  if (result != 0) {
    return result;
  }

  // It is not possible to receive clear code as first code.
  // So we can compare first code with alphabet length.

  if (code >= LZWS_ALPHABET_LENGTH) {
    if (!state_ptr->quiet) {
      LZWS_PRINTF_ERROR("received invalid first code: " FAST_CODE_FORMAT, code)
    }

    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }

  // It is possible to keep current code as is.
  // Algorithm won't touch it without reinitialization.

  state_ptr->prefix_code = code;
  state_ptr->status      = LZWS_DECOMPRESSOR_WRITE_FIRST_SYMBOL;

  return 0;
}

static inline lzws_code_fast_t get_next_code(lzws_decompressor_state_t* state_ptr)
{
  if (state_ptr->last_used_code == state_ptr->last_used_max_code) {
    uint_fast8_t last_used_code_bit_length = ++state_ptr->last_used_code_bit_length;
    state_ptr->last_used_max_code          = lzws_get_mask_for_last_bits(last_used_code_bit_length);
  }

  return ++state_ptr->last_used_code;
}

lzws_result_t lzws_decompressor_read_next_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  lzws_code_fast_t code;

  lzws_result_t result = lzws_decompressor_read_code(state_ptr, source_ptr, source_length_ptr, &code);
  if (result != 0) {
    return result;
  }

  bool is_dictionary_full = lzws_decompressor_is_dictionary_full(state_ptr);
  bool quiet              = state_ptr->quiet;

  if (state_ptr->block_mode && code == LZWS_CLEAR_CODE) {
    if (!is_dictionary_full) {
      if (!quiet) {
        LZWS_PRINT_ERROR("received clear code when dictionary is not full")
      }

      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }

    lzws_decompressor_clear_state(state_ptr);

    // It is possible to keep prefix_code as is.
    // Algorithm won't touch it without reinitialization.

    state_ptr->status = LZWS_DECOMPRESSOR_READ_FIRST_CODE;

    return 0;
  }

  if (is_dictionary_full) {
    lzws_decompressor_write_code_to_dictionary_wrapper(state_ptr, code);
  }
  else {
    lzws_code_fast_t next_code = get_next_code(state_ptr);
    if (code > next_code) {
      if (!quiet) {
        LZWS_PRINTF_ERROR("received code greater than next code: " FAST_CODE_FORMAT, code)
      }

      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }

    lzws_decompressor_add_code_to_dictionary_wrapper(state_ptr, state_ptr->prefix_code, code, next_code);
  }

  state_ptr->prefix_code = code;
  state_ptr->status      = LZWS_DECOMPRESSOR_WRITE_DICTIONARY;

  return 0;
}

lzws_result_t lzws_decompressor_verify_empty_source_remainder(lzws_decompressor_state_t* state_ptr)
{
  uint_fast8_t source_remainder            = state_ptr->source_remainder;
  uint_fast8_t source_remainder_bit_length = state_ptr->source_remainder_bit_length;

  if (source_remainder_bit_length != 0 && source_remainder != 0) {
    if (!state_ptr->quiet) {
      LZWS_PRINTF_ERROR("source remainder is not empty, bit length: %u, value: %u", source_remainder_bit_length, source_remainder)
    }

    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }

  return 0;
}
