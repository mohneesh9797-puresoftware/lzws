// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_UTILS_H)
#define LZWS_UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

#undef LZWS_INLINE
#if defined(LZWS_UTILS_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_fill_array(void* array, size_t size_of_item, size_t length, void* value) {
  uint8_t* bytes      = value;
  uint8_t  first_byte = bytes[0];

  if (size_of_item == 1) {
    memset(array, first_byte, length);
    return;
  }

  // Size of item will always be > 1.
  bool all_bytes_are_identical = true;

  for (size_t byte_index = 1; byte_index < size_of_item; byte_index++) {
    if (bytes[byte_index] != first_byte) {
      all_bytes_are_identical = false;
      break;
    }
  }

  if (all_bytes_are_identical) {
    memset(array, first_byte, size_of_item * length);
    return;
  }

  for (size_t index = 0; index < length; index++) {
    memcpy(array + size_of_item * index, value, size_of_item);
  }
}

LZWS_INLINE void* lzws_allocate_array(size_t size_of_item, size_t length, bool default_value_required, uint default_value) {
  size_t size = size_of_item * length;

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

  lzws_fill_array(array, size_of_item, length, &default_value);

  return array;
}

LZWS_INLINE void lzws_read_byte(uint8_t** source, size_t* source_length, uint8_t* byte) {
  *byte = **source;
  (*source)++;
  (*source_length)--;
}

LZWS_INLINE void lzws_write_byte(uint8_t** destination, size_t* destination_length, uint8_t byte) {
  **destination = byte;
  (*destination)++;
  (*destination_length)--;
}

LZWS_INLINE uint8_t lzws_reverse_byte(uint8_t byte) {
  return LZWS_REVERSE_BITS_TABLE[byte];
}

#endif // LZWS_UTILS_H
