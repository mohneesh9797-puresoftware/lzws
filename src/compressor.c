// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

// Please read "doc/compressor.txt" and "doc/output_compatibility".

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "compressor.h"

// -- utils --

static void read_byte(uint8_t** source, size_t* source_length, uint8_t* byte) {
  *byte = **source;
  (*source)++;
  (*source_length)--;
}

static void write_byte(uint8_t** destination, size_t* destination_length, uint8_t byte) {
  **destination = byte;
  (*destination)++;
  (*destination_length)--;
}

static void* allocate_array(size_t length, size_t size_of_item, bool default_value_required, uint8_t default_value) {
  size_t size = length * size_of_item;

  if (!default_value_required) {
    return malloc(size);
  }

  if (default_value == 0) {
    return calloc(1, size);
  }

  void* array = malloc(size);
  if (array == NULL) {
    return NULL;
  }

  memset(array, default_value, length);
  return array;
}

// -- state --

lzws_result_t lzws_compressor_get_initial_state(lzws_compressor_state_t** result, uint8_t max_code_bits, bool block_mode) {
  if (max_code_bits < LZWS_LOWEST_MAX_CODE_BITS || max_code_bits > LZWS_BIGGEST_MAX_CODE_BITS) {
    return LZWS_COMPRESSOR_INVALID_MAX_CODE_BITS;
  }

  lzws_compressor_state_t* state = malloc(sizeof(lzws_compressor_state_t));
  if (state == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  state->status = LZWS_COMPRESSOR_WRITE_HEADER;

  state->max_code_bits = max_code_bits;
  state->block_mode    = block_mode;

  state->first_child_codes  = NULL;
  state->next_sibling_codes = NULL;
  state->symbol_by_codes    = NULL;

  state->prefix_code         = LZWS_UNDEFINED_CODE;
  state->current_code        = LZWS_UNDEFINED_CODE;
  state->last_used_code      = LZWS_LAST_USED_CODE;
  state->last_used_code_bits = LZWS_LOWEST_MAX_CODE_BITS;

  state->remainder      = 0;
  state->remainder_bits = 0;

  *result = state;

  return 0;
}

void lzws_compressor_free_state(lzws_compressor_state_t* state) {
  lzws_code_t* first_child_codes = state->first_child_codes;
  if (first_child_codes != NULL) {
    free(first_child_codes);
  }

  lzws_code_t* next_sibling_codes = state->next_sibling_codes;
  if (next_sibling_codes != NULL) {
    free(next_sibling_codes);
  }

  uint8_t* symbol_by_codes = state->symbol_by_codes;
  if (symbol_by_codes != NULL) {
    free(symbol_by_codes);
  }

  free(state);
}

// -- header --

lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length) {
  if (*destination_length < 2) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_0);
  write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_1);

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

  write_byte(destination, destination_length, byte);

  return 0;
}

// -- dictionary --

static lzws_result_t allocate_dictionary(lzws_compressor_state_t* state) {
  size_t total_codes = 1 << state->max_code_bits;

  lzws_code_t* first_child_codes = allocate_array(total_codes, sizeof(lzws_code_t), true, LZWS_UNDEFINED_CODE);
  if (first_child_codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  lzws_code_t* next_sibling_codes = allocate_array(total_codes - LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET, sizeof(lzws_code_t), true, LZWS_UNDEFINED_CODE);
  if (next_sibling_codes == NULL) {
    // "first_child_codes" was allocated, need to free it.
    free(first_child_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  uint8_t* symbol_by_codes = allocate_array(total_codes - LZWS_SYMBOL_BY_CODES_INDEX_OFFSET, sizeof(uint8_t), false, 0);
  if (symbol_by_codes == NULL) {
    // "first_child_codes" and "next_sibling_codes" were allocated, need to free it.
    free(first_child_codes);
    free(next_sibling_codes);

    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  state->first_child_codes  = first_child_codes;
  state->next_sibling_codes = next_sibling_codes;
  state->symbol_by_codes    = symbol_by_codes;

  return 0;
}

static void clear_dictionary(lzws_compressor_state_t* state) {
  size_t total_codes = 1 << state->max_code_bits;

  memset(state->first_child_codes, LZWS_UNDEFINED_CODE, total_codes);
  memset(state->next_sibling_codes, LZWS_UNDEFINED_CODE, total_codes - LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET);

  // We don't need to clear "symbol_by_codes", they were considered uninitialized.
  // Algorithm will access only initialized codes.

  state->last_used_code      = LZWS_LAST_USED_CODE;
  state->last_used_code_bits = LZWS_LOWEST_MAX_CODE_BITS;
}

// -- read symbol --

static lzws_code_t get_new_code_for_symbol(lzws_compressor_state_t* state, uint8_t symbol) {
  lzws_code_t max_code = (1 << state->max_code_bits) - 1;

  if (state->last_used_code == max_code) {
    // Dictionary overflow.
    // We don't need to send clear code, decoder knows about it.
    clear_dictionary(state);

    return LZWS_UNDEFINED_CODE;
  }

  state->last_used_code++;
  lzws_code_t new_code = state->last_used_code;

  lzws_code_t max_last_used_code = (1 << state->last_used_code_bits) - 1;
  if (new_code == max_last_used_code) {
    state->last_used_code_bits++;
  }

  // Adding symbol for new code.
  lzws_code_t symbol_by_code_index             = new_code - LZWS_SYMBOL_BY_CODES_INDEX_OFFSET;
  state->symbol_by_codes[symbol_by_code_index] = symbol;

  return new_code;
}

static lzws_code_t get_code_for_symbol(lzws_compressor_state_t* state, uint8_t symbol, bool* is_added) {
  lzws_code_t  prefix_code       = state->prefix_code;
  lzws_code_t* first_child_codes = state->first_child_codes;

  lzws_code_t new_code;

  lzws_code_t first_child_code = first_child_codes[prefix_code];
  if (first_child_code == LZWS_UNDEFINED_CODE) {
    // First child is not found.
    // We will try to add first child.

    new_code = get_new_code_for_symbol(state, symbol);
    if (new_code == LZWS_UNDEFINED_CODE) {
      // Dictionary was cleared.
      return LZWS_UNDEFINED_CODE;
    }

    // Adding first child.
    first_child_codes[prefix_code] = new_code;
    *is_added                      = true;

    return new_code;
  }

  // We have some first child.
  // We need to find target symbol in next siblings.

  lzws_code_t* next_sibling_codes = state->next_sibling_codes;
  uint8_t*     symbol_by_codes    = state->symbol_by_codes;

  // We know that "first_child_code" is not undefined, so it is better to use do + while for it.
  lzws_code_t next_sibling_code = first_child_code;

  do {
    lzws_code_t symbol_by_code_index = next_sibling_code - LZWS_SYMBOL_BY_CODES_INDEX_OFFSET;
    if (symbol_by_codes[symbol_by_code_index] == symbol) {
      // We found target symbol.
      *is_added = false;

      return next_sibling_code;
    }

    lzws_code_t next_sibling_code_index = next_sibling_code - LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET;
    next_sibling_code                   = next_sibling_codes[next_sibling_code_index];
  } while (next_sibling_code != LZWS_UNDEFINED_CODE);

  // Next sibling is not found.
  // We will try to add next sibling.

  new_code = get_new_code_for_symbol(state, symbol);
  if (new_code == LZWS_UNDEFINED_CODE) {
    // Dictionary was cleared.
    return LZWS_UNDEFINED_CODE;
  }

  // Adding next sibling.
  lzws_code_t new_sibling_code_index = new_code - LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET;

  first_child_codes[prefix_code]             = new_code;
  next_sibling_codes[new_sibling_code_index] = first_child_code;
  *is_added                                  = true;

  return new_code;
}

static lzws_result_t read_next_symbol(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length) {
  if (*source_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint8_t symbol;
  read_byte(source, source_length, &symbol);

  lzws_code_t prefix_code = state->prefix_code;

  if (prefix_code == LZWS_UNDEFINED_CODE) {
    // Symbol is new prefix code.
    state->prefix_code = symbol;

    // We don't need to change status, algorithm wants next symbol.

    return 0;
  }

  lzws_code_t current_code = state->current_code;

  bool        is_added;
  lzws_code_t code = get_code_for_symbol(state, symbol, &is_added);

  if (code == LZWS_UNDEFINED_CODE) {
    // Dictionary was cleared, we are not able to get code for symbol.

    // We need to write current code if it exists.
    if (current_code != LZWS_UNDEFINED_CODE) {
      state->status = LZWS_COMPRESSOR_WRITE_CURRENT_CODE;
    }

    // Symbol is new prefix code.
    state->prefix_code = symbol;

    // We don't need to change status, algorithm wants next symbol.

    return 0;
  }

  if (is_added) {
    // We can't find code for symbol.
    // Current code is prefix code.
    // We need to write it.

    state->current_code = prefix_code;
    state->status       = LZWS_COMPRESSOR_WRITE_CURRENT_CODE;

    // Symbol is new prefix code.
    state->prefix_code = symbol;

    return 0;
  }

  // We found code for symbol, it becomes new prefix code.
  state->prefix_code = code;

  // We don't need to change status, algorithm wants next symbol.

  return 0;
}

// -- write code --

static lzws_result_t write_current_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  uint8_t code_bits      = state->last_used_code_bits;
  uint8_t remainder_bits = state->remainder_bits;

  // Destination bytes will always be >= 1.
  uint8_t destination_bytes = (code_bits + remainder_bits) / 8;

  if (*destination_length < destination_bytes) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  lzws_code_t code = state->current_code;

  uint8_t byte;

  if (remainder_bits != 0) {
    uint8_t remainder      = state->remainder;
    uint8_t code_mask_bits = 8 - remainder_bits;

    // All bits of the mask are 1.
    uint8_t code_mask = (1 << code_mask_bits) - 1;

    // Code bits here will always be > 8.
    byte = ((code & code_mask) << remainder_bits) | remainder;
    code >>= code_mask_bits;
    code_bits -= code_mask_bits;

    write_byte(destination, destination_length, byte);
  }

  while (code_bits >= 8) {
    byte = code & 0xff;
    code >>= 8;
    code_bits -= 8;

    write_byte(destination, destination_length, byte);
  }

  state->current_code   = LZWS_UNDEFINED_CODE;
  state->remainder      = code;
  state->remainder_bits = code_bits;

  return 0;
}

static lzws_result_t write_prefix_code(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  // We need to write prefix code if it exists.
  lzws_code_t prefix_code = state->prefix_code;

  if (prefix_code == LZWS_UNDEFINED_CODE) {
    return 0;
  }

  state->current_code = prefix_code;

  lzws_result_t result = write_current_code(state, destination, destination_length);
  if (result != 0) {
    return result;
  }

  state->prefix_code = LZWS_UNDEFINED_CODE;

  return 0;
}

static lzws_result_t write_remainder(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  if (state->remainder_bits == 0) {
    return 0;
  }

  if (*destination_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  write_byte(destination, destination_length, state->remainder);

  state->remainder      = 0;
  state->remainder_bits = 0;

  return 0;
}

// -- compress --

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result;

  while (true) {
    switch (state->status) {
      case LZWS_COMPRESSOR_WRITE_HEADER:
        result        = write_header(state, destination, destination_length);
        state->status = LZWS_COMPRESSOR_ALLOCATE_DICTIONARY;
        break;
      case LZWS_COMPRESSOR_ALLOCATE_DICTIONARY:
        result        = allocate_dictionary(state);
        state->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;
        break;
      case LZWS_COMPRESSOR_READ_NEXT_SYMBOL:
        result = read_next_symbol(state, source, source_length);
        // status will be assigned automatically.
        break;
      case LZWS_COMPRESSOR_WRITE_CURRENT_CODE:
        result        = write_current_code(state, destination, destination_length);
        state->status = LZWS_COMPRESSOR_READ_NEXT_SYMBOL;
        break;
      default:
        return LZWS_COMPRESSOR_UNKNOWN_STATUS;
    }

    if (result != 0) {
      return result;
    }
  }
}

lzws_result_t lzws_compressor_flush(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  lzws_result_t result = write_prefix_code(state, destination, destination_length);
  if (result != 0) {
    return result;
  }

  return write_remainder(state, destination, destination_length);
}
