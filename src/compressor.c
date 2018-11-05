// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

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

static void* allocate_array(size_t length, size_t size_of_item, bool default_value_required, uint default_value) {
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

// -- magic header --

lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length) {
  if (*destination_length < 2) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_0);
  write_byte(destination, destination_length, LZWS_MAGIC_HEADER_BYTE_1);

  return 0;
}

// -- state --

lzws_result_t lzws_get_initial_compressor_state(lzws_compressor_state_t** result, uint8_t max_bits, bool block_mode) {
  if (max_bits < LZWS_LOWEST_MAX_BITS || max_bits > LZWS_BIGGEST_MAX_BITS) {
    return LZWS_COMPRESSOR_INVALID_MAX_BITS;
  }

  lzws_compressor_state_t* state = malloc(sizeof(lzws_compressor_state_t));
  if (state == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  state->status = LZWS_COMPRESSOR_WRITE_HEADER;

  state->max_bits   = max_bits;
  state->block_mode = block_mode;

  state->first_child_codes  = NULL;
  state->next_sibling_codes = NULL;
  state->symbol_by_codes    = NULL;

  state->prev_code = LZWS_PREV_FIRST_CODE;

  *result = state;

  return 0;
}

void lzws_free_compressor_state(lzws_compressor_state_t* state) {
  lzws_code_t* first_child_codes = state->first_child_codes;
  if (first_child_codes != NULL) free(first_child_codes);

  lzws_code_t* next_sibling_codes = state->next_sibling_codes;
  if (next_sibling_codes != NULL) free(next_sibling_codes);

  uint8_t* symbol_by_codes = state->symbol_by_codes;
  if (symbol_by_codes != NULL) free(symbol_by_codes);

  free(state);
}

// -- compress --

// 1 byte for "max_bits" and "block_mode".

static lzws_result_t write_header(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length) {
  uint8_t byte = state->max_bits;
  if (state->block_mode) {
    byte = byte | LZWS_BLOCK_MODE;
  }

  if (*destination_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION;
  }

  write_byte(destination, destination_length, byte);

  state->status = LZWS_COMPRESSOR_ALLOCATE_DICTIONARY;

  return 0;
}

static lzws_result_t allocate_dictionary(lzws_compressor_state_t* state) {
  size_t total_codes = 1 << state->max_bits;

  lzws_code_t* first_child_codes = allocate_array(total_codes, sizeof(lzws_code_t), true, LZWS_UNDEFINED_CODE);
  if (first_child_codes == NULL) {
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  lzws_code_t* next_sibling_codes = allocate_array(total_codes - LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET, sizeof(lzws_code_t), true, LZWS_UNDEFINED_CODE);
  if (next_sibling_codes == NULL) {
    free(first_child_codes);
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  uint8_t* symbol_by_codes = allocate_array(total_codes - LZWS_SYMBOL_BY_CODES_INDEX_OFFSET, sizeof(uint8_t), false, 0);
  if (symbol_by_codes == NULL) {
    free(first_child_codes);
    free(next_sibling_codes);
    return LZWS_COMPRESSOR_ALLOCATE_FAILED;
  }

  state->first_child_codes  = first_child_codes;
  state->next_sibling_codes = next_sibling_codes;
  state->symbol_by_codes    = symbol_by_codes;

  state->status = LZWS_COMPRESSOR_GET_FIRST_SYMBOL;

  return 0;
}

static lzws_result_t get_first_symbol(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length) {
  if (*source_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint8_t symbol;
  read_byte(source, source_length, &symbol);

  state->current_code = symbol;

  state->status = LZWS_COMPRESSOR_PROCESS_NEXT_SYMBOL;

  return 0;
}

static void clear_dictionary(lzws_compressor_state_t* state) {
  size_t total_codes = 1 << state->max_bits;

  memset(state->first_child_codes, LZWS_UNDEFINED_CODE, total_codes);
  memset(state->next_sibling_codes, LZWS_UNDEFINED_CODE, total_codes - LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET);
  // We don't need to clear "symbol_by_codes".
  // Algorithm will access only initialized codes.

  state->prev_code = LZWS_PREV_FIRST_CODE;
}

static lzws_code_t get_next_code_for_symbol(lzws_compressor_state_t* state, uint8_t symbol) {
  size_t total_codes = 1 << state->max_bits;

  if (state->prev_code == total_codes - 1) {
    // Dictionary overflow.
    // We don't need to send clear code, decoder knows about it.
    clear_dictionary(state);

    // Dictionary was cleared.
    // We received new first symbol.
    state->current_code = symbol;
    state->status       = LZWS_COMPRESSOR_PROCESS_NEXT_SYMBOL;

    return LZWS_UNDEFINED_CODE;
  }

  state->prev_code++;
  lzws_code_t code = state->prev_code;

  uint8_t* symbol_by_codes = state->symbol_by_codes;
  symbol_by_codes[code]    = symbol;

  return code;
}

static void get_code(lzws_compressor_state_t* state, uint8_t symbol, bool* is_added) {
  lzws_code_t  current_code      = state->current_code;
  lzws_code_t* first_child_codes = state->first_child_codes;

  lzws_code_t code = first_child_codes[current_code];
  if (code == LZWS_UNDEFINED_CODE) {
    // We are going to add first child.

    lzws_code_t next_code = get_next_code_for_symbol(state, symbol);
    if (next_code == LZWS_UNDEFINED_CODE) {
      return; // Dictionary was cleared.
    }

    first_child_codes[current_code] = next_code;
    return;
  }

  // We have some first child.
  // We need to find target symbol in next siblings.

  lzws_code_t* next_sibling_codes = state->next_sibling_codes;
  uint8_t*     symbol_by_codes    = state->symbol_by_codes;

  while (code != LZWS_UNDEFINED_CODE) {
    if (symbol_by_codes[code] == symbol) {
      return;
    }

    code = next_sibling_codes[code];
  }

  lzws_code_t next_code = get_next_code_for_symbol(state, symbol);
  if (next_code == LZWS_UNDEFINED_CODE) {
    return; // Dictionary was cleared.
  }
}

static lzws_result_t process_next_symbol(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length) {
  if (*source_length < 1) {
    return LZWS_COMPRESSOR_NEEDS_MORE_SOURCE;
  }

  uint8_t symbol;
  read_byte(source, source_length, &symbol);

  lzws_code_t* first_child_codes  = state->first_child_codes;
  lzws_code_t* next_sibling_codes = state->next_sibling_codes;
  uint8_t*     symbol_by_codes    = state->symbol_by_codes;

  uint8_t current_code = state->current_code;

  // uint16_t index = first_references[prefix]; // coding table index
  // if (index == 0) {
  //   // No longer strings are based on the current prefix.
  //   // So now the current prefix plus the new byte will be the next string.
  //
  //   first_references[prefix] = next_reference;
  // } else {
  //   // If any longer strings are built on the current prefix.
  //
  //   while (true) {
  //     uint8_t current_terminator = terminators[index - 256];
  //     if (current_terminator == symbol) { // We found a matching string.
  //       prefix = index;                   // So we just update the prefix to that string and continue without sending anything.
  //       break;
  //     }
  //
  //     uint16_t current_next_reference = next_references[index - 256];
  //     if (current_next_reference == 0) {               // This string did not match the new character and there aren't any more.
  //       next_references[index - 256] = next_reference; // So we'll add a new string and point to it with "next_reference".
  //       index                        = 0;
  //       break;
  //     }
  //   }
  // }

  return 0;
}

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
      case LZWS_COMPRESSOR_GET_FIRST_SYMBOL:
        result = get_first_symbol(state, source, source_length);
        break;
      case LZWS_COMPRESSOR_PROCESS_NEXT_SYMBOL:
        result = process_next_symbol(state, source, source_length, destination, destination_length);
        break;
      default:
        return LZWS_COMPRESSOR_UNKNOWN_STATUS;
    }

    if (result != 0) return result;
  }
}
