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

LZWS_INLINE uint_fast8_t lzws_reverse_byte(uint_fast8_t byte) {
  return LZWS_REVERSE_BITS[byte];
}

LZWS_INLINE uint_fast32_t lzws_get_power_of_two(uint_fast8_t power) {
  return LZWS_POWERS_OF_TWO[power];
}

LZWS_INLINE uint_fast16_t lzws_get_bit_mask(uint_fast8_t bits) {
  return LZWS_BIT_MASKS[bits];
}

LZWS_INLINE void lzws_read_byte(uint8_t** source_ptr, size_t* source_length_ptr, uint_fast8_t* byte) {
  *byte = **source_ptr;
  (*source_ptr)++;
  (*source_length_ptr)--;
}

LZWS_INLINE void lzws_write_byte(uint8_t** destination_ptr, size_t* destination_length_ptr, uint_fast8_t byte) {
  **destination_ptr = byte;
  (*destination_ptr)++;
  (*destination_length_ptr)--;
}

LZWS_INLINE void lzws_fill_array(void* array, size_t size_of_item, size_t length, void* item, bool item_is_zero) {
  uint8_t* bytes      = item;
  uint8_t  first_byte = bytes[0];

  if (size_of_item == 1) {
    memset(array, first_byte, length);
    return;
  }

  if (item_is_zero) {
    memset(array, first_byte, size_of_item * length);
    return;
  }

  for (size_t index = 0; index < length; index++) {
    memcpy((uint8_t*)array + size_of_item * index, item, size_of_item);
  }
}

LZWS_INLINE void* lzws_allocate_array(uint_fast8_t size_of_item, size_t length, void* item, bool item_is_zero) {
  size_t size = size_of_item * length;

  if (item_is_zero) {
    return calloc(1, size);
  }

  void* array = malloc(size);
  if (array == NULL) {
    return NULL;
  }

  lzws_fill_array(array, size_of_item, length, item, false);

  return array;
}

#endif // LZWS_UTILS_H
