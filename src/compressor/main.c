// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "../constants.h"
#include "../utils.h"

#include "common.h"
#include "main.h"

// -- header --

lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length) {
  if (*destination_length < 2) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_0);
  lzws_write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_1);

  return 0;
}

static lzws_result_t write_header(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  // Writing 1 byte for "max_code_bits" and "block_mode".
  if (*destination_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  uint8_t byte = state->max_code_bits;
  if (state->block_mode) {
    byte = byte | LZWS_BLOCK_MODE;
  }

  lzws_write_byte(destination, destination_length, byte);

  state->status = LZWS_COMPRESSOR_ALLOCATE_DICTIONARY;

  return 0;
}

// -- dictionary --

static lzws_result_t allocate_dictionary(lzws_compressor_state_t* state) {
  lzws_result_t result = lzws_compressor_dictionary_allocate(&state->dictionary, state->max_code_bits, state->initial_code_offset);
  if (result != 0) {
    return result;
  }

  state->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;

  return 0;
}

// static void clear_dictionary(lzws_compressor_state_t* state) {
// }

// -- read symbol --

static lzws_code_t get_new_code(lzws_compressor_state_t* state, uint8_t symbol) {
  if (state->last_used_code == state->max_code) {
    // Dictionary is full.
    return LZWS_UNDEFINED_CODE;
  }

  state->last_used_code++;
  lzws_code_t new_code = state->last_used_code;

  lzws_code_t first_code_for_next_code_bits = LZWS_POWERS_OF_TWO[state->last_used_code_bits];
  if (new_code == first_code_for_next_code_bits) {
    state->last_used_code_bits++;
  }

  return new_code;
}

static lzws_result_t read_next_symbol(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length) {
  if (*source_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint8_t symbol;
  lzws_read_byte(source, source_length, &symbol);

  lzws_code_t current_code = state->current_code;
  if (current_code == LZWS_UNDEFINED_CODE) {
    // Symbol is new current code.
    state->current_code = symbol;

    // We don't need to change status, algorithm wants next symbol.
    return 0;
  }

  lzws_code_t next_code = lzws_compressor_dictionary_get_code(&state->dictionary, state->initial_code_offset, current_code, symbol);
  if (next_code != LZWS_UNDEFINED_CODE) {
    // We found next code, it becomes new current code.
    state->current_code = next_code;

    // We don't need to change status, algorithm wants next symbol.
    return 0;
  }

  // Dictionary can't find next code.

  lzws_code_t new_code = get_new_code(state, symbol);
  if (new_code != LZWS_UNDEFINED_CODE) {
    lzws_compressor_dictionary_get_code(&state->dictionary, state->initial_code_offset, current_code, new_code);
  }

  // We need to write current code.

  state->next_symbol = symbol;
  state->status      = LZWS_COMPRESSOR_WRITE_CURRENT_CODE;

  return 0;
}

// -- compress --

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result;

  while (true) {
    switch (state->status) {
      case LZWS_COMPRESSOR_WRITE_HEADER:
        result = write_header(state, destination, destination_length);
        break;
      case LZWS_COMPRESSOR_ALLOCATE_DICTIONARY:
        result = allocate_dictionary(state);
        break;
      case LZWS_COMPRESSOR_READ_NEXT_SYMBOL:
        result = read_next_symbol(state, source, source_length);
        break;
      case LZWS_COMPRESSOR_WRITE_CURRENT_CODE:
        // result = write_current_code(state, destination, destination_length);
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
