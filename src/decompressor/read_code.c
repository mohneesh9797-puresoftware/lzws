// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "read_code.h"
#include "common.h"
#include "utils.h"

static inline void add_byte(lzws_code_fast_t* code_ptr, uint_fast8_t code_bits, uint_fast8_t byte, bool msb) {
  lzws_code_fast_t code = *code_ptr;

  if (msb) {
    // Code is sitting on the top.
    code = (code << 8) | byte;
  } else {
    // Code is sitting on the bottom.
    code = (byte << code_bits) | code;
  }

  *code_ptr = code;
}

static inline void add_byte_with_remainder(
  lzws_code_fast_t* code_ptr, uint_fast8_t code_bits, uint_fast8_t target_code_bits,
  uint_fast8_t byte, uint_fast8_t* source_remainder_ptr, uint_fast8_t* source_remainder_bits_ptr,
  bool msb) {
  //

  uint_fast8_t code_part_bits = target_code_bits - code_bits;
  if (code_part_bits == 8) {
    add_byte(code_ptr, code_bits, byte, msb);

    *source_remainder_ptr      = 0;
    *source_remainder_bits_ptr = 0;

    return;
  }

  lzws_code_fast_t code = *code_ptr;
  uint_fast8_t     code_part;

  uint_fast8_t source_remainder;
  uint_fast8_t source_remainder_bits = 8 - code_part_bits;

  if (msb) {
    // Taking first bits from byte.
    code_part = byte >> source_remainder_bits;

    // Last bits of byte is a remainder.
    source_remainder = byte & lzws_get_bit_mask(source_remainder_bits);

    // Code is sitting on the top.
    code = (code << code_part_bits) | code_part;
  } else {
    // Taking last bits from byte.
    code_part = byte & lzws_get_bit_mask(code_part_bits);

    // First bits of byte is a remainder.
    source_remainder = byte >> code_part_bits;

    // Code is sitting on the bottom.
    code = (code_part << code_bits) | code;
  }

  *code_ptr                  = code;
  *source_remainder_ptr      = source_remainder;
  *source_remainder_bits_ptr = source_remainder_bits;
}

static inline uint_fast8_t get_target_code_bits(lzws_decompressor_state_t* state_ptr) {
  uint_fast8_t last_used_code_bits = state_ptr->last_used_code_bits;

  if (lzws_decompressor_is_dictionary_full(state_ptr) || state_ptr->last_used_code != state_ptr->last_used_max_code) {
    return last_used_code_bits;
  }

  return last_used_code_bits + 1;
}

lzws_result_t lzws_decompressor_read_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, lzws_code_fast_t* code_ptr) {
  uint_fast8_t target_code_bits      = get_target_code_bits(state_ptr);
  uint_fast8_t source_remainder_bits = state_ptr->source_remainder_bits;

  // Target code bits will always be >= 8.
  // Source remainder bits will always be <= 7.
  // So source bytes will always be >= 1.
  uint_fast8_t source_bytes = lzws_ceil_bits_to_bytes(target_code_bits - source_remainder_bits);
  if (*source_length_ptr < source_bytes) {
    return LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE;
  }

  lzws_code_fast_t code      = state_ptr->source_remainder;
  uint_fast8_t     code_bits = source_remainder_bits;
  bool             msb       = state_ptr->msb;

  uint_fast8_t byte;

  while (source_bytes != 1) {
    lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);
    add_byte(&code, code_bits, byte, msb);

    code_bits += 8;
    source_bytes--;
  }

  lzws_decompressor_read_byte(state_ptr, source_ptr, source_length_ptr, &byte);
  add_byte_with_remainder(
    &code, code_bits, target_code_bits,
    byte, &state_ptr->source_remainder, &state_ptr->source_remainder_bits,
    msb);

  *code_ptr = code;

  return 0;
}

lzws_result_t lzws_decompressor_read_first_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  lzws_code_fast_t code;

  lzws_result_t result = lzws_decompressor_read_code(state_ptr, source_ptr, source_length_ptr, &code);
  if (result != 0) {
    return result;
  }

  // It is not possible to receive clear code as first code.
  // So we can compare first code with alphabet length.

  if (code >= LZWS_ALPHABET_LENGTH) {
    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }

  // It is possible to keep current code as is.
  // Algorithm won't touch it without reinitialization.

  state_ptr->prefix_code = code;
  state_ptr->status      = LZWS_DECOMPRESSOR_WRITE_FIRST_SYMBOL;

  return 0;
}

static inline lzws_result_t prepare_code_for_writing(lzws_decompressor_state_t* state_ptr, lzws_code_fast_t code) {
  // It is not possible to receive more than max code.
  // So we can compare code only with expected next code when dictionary is not full.

  if (lzws_decompressor_is_dictionary_full(state_ptr)) {
    if (code >= LZWS_ALPHABET_LENGTH) {
      // TODO prepare code for writing.
    }
    return 0;
  }

  lzws_code_fast_t last_used_code     = state_ptr->last_used_code;
  lzws_code_fast_t expected_next_code = last_used_code + 1;

  if (code > expected_next_code) {
    return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
  }

  if (code == expected_next_code) {
    // Code can be equal to expected next code only when prefix code equals to last used code.
    if (state_ptr->prefix_code != last_used_code) {
      return LZWS_DECOMPRESSOR_CORRUPTED_SOURCE;
    }
    // TODO prepare special code for writing.
  } else if (code >= LZWS_ALPHABET_LENGTH) {
    // TODO prepare code for writing.
  }

  return 0;
}

lzws_result_t lzws_decompressor_read_next_code(lzws_decompressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr) {
  lzws_code_fast_t code;

  lzws_result_t result = lzws_decompressor_read_code(state_ptr, source_ptr, source_length_ptr, &code);
  if (result != 0) {
    return result;
  }

  result = prepare_code_for_writing(state_ptr, code);
  if (result != 0) {
    return result;
  }

  state_ptr->current_code = code;
  state_ptr->status       = LZWS_DECOMPRESSOR_WRITE_CURRENT_CODE;

  return 0;
}
