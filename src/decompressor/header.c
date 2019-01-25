// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../log.h"
#include "../utils.h"

#include "common.h"
#include "header.h"

lzws_result_t lzws_decompressor_read_magic_header(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  if (*source_length_ptr < 2) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint_fast8_t byte;
  lzws_read_byte(source_ptr, source_length_ptr, &byte);

  if (byte != LZWS_FIRST_MAGIC_HEADER_BYTE) {
    if (!state_ptr->quiet) {
      LZWS_PRINTF_ERROR("received invalid first magic header byte: %u", byte)
    }

    return LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER;
  }

  lzws_read_byte(source_ptr, source_length_ptr, &byte);

  if (byte != LZWS_SECOND_MAGIC_HEADER_BYTE) {
    if (!state_ptr->quiet) {
      LZWS_PRINTF_ERROR("received invalid second magic header byte: %u", byte)
    }

    return LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER;
  }

  return 0;
}

lzws_result_t lzws_decompressor_read_header(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  // Reading first byte for max code bit length and block mode.
  if (*source_length_ptr < 1) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint_fast8_t byte;
  lzws_read_byte(source_ptr, source_length_ptr, &byte);

  uint_fast8_t max_code_bit_length = byte & LZWS_MAX_CODE_BIT_MASK;
  if (max_code_bit_length < LZWS_LOWEST_MAX_CODE_BIT_LENGTH || max_code_bit_length > LZWS_BIGGEST_MAX_CODE_BIT_LENGTH) {
    if (!state_ptr->quiet) {
      LZWS_PRINTF_ERROR("received invalid max code bit length: %u", max_code_bit_length)
    }

    return LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH;
  }

  bool             block_mode        = (byte & LZWS_BLOCK_MODE) != 0;
  lzws_code_fast_t initial_used_code = lzws_get_initial_used_code(block_mode);

  state_ptr->block_mode = block_mode;

  state_ptr->initial_used_code = initial_used_code;
  state_ptr->max_code          = lzws_get_mask_for_last_bits(max_code_bit_length);

  state_ptr->last_used_code            = initial_used_code;
  state_ptr->last_used_max_code        = lzws_get_mask_for_last_bits(LZWS_LOWEST_MAX_CODE_BIT_LENGTH);
  state_ptr->last_used_code_bit_length = LZWS_LOWEST_MAX_CODE_BIT_LENGTH;

  // It is possible to keep prefix code uninitialized.

  state_ptr->status = LZWS_DECOMPRESSOR_ALLOCATE_DICTIONARY;

  return 0;
}
