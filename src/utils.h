// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_UTILS_H)
#define LZWS_UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

inline uint_fast8_t lzws_get_byte_with_reversed_bits(uint_fast8_t byte)
{
  return LZWS_BYTES_WITH_REVERSED_BITS[byte];
}

inline uint_fast16_t lzws_get_mask_for_last_bits(uint_fast8_t bit_length)
{
  return LZWS_MASKS_FOR_LAST_BITS[bit_length];
}

inline uint_fast16_t lzws_get_max_value_for_bits(uint_fast8_t bit_length)
{
  return lzws_get_mask_for_last_bits(bit_length);
}

inline size_t lzws_floor_bit_length_to_byte_length(size_t bit_length)
{
  return bit_length >> 3;
}

inline size_t lzws_ceil_bit_length_to_byte_length(size_t bit_length)
{
  return (bit_length + 7) >> 3;
}

inline void lzws_read_byte(uint_fast8_t* byte_ptr, uint8_t** source_ptr, size_t* source_length_ptr)
{
  *byte_ptr = **source_ptr;
  (*source_ptr)++;
  (*source_length_ptr)--;
}

inline void lzws_write_byte(uint_fast8_t byte, uint8_t** destination_ptr, size_t* destination_length_ptr)
{
  **destination_ptr = byte;
  (*destination_ptr)++;
  (*destination_length_ptr)--;
}

// Keeping universal fill and allocate functions despite the losses of coverage percent.

inline void lzws_fill_array(void* array, size_t size_of_item, size_t length, void* item_ptr, bool item_bytes_are_identical)
{
  uint8_t* bytes      = item_ptr;
  uint8_t  first_byte = bytes[0];

  if (item_bytes_are_identical) {
    memset(array, first_byte, size_of_item * length);
    return;
  }

  for (size_t index = 0; index < length; index++) {
    memcpy((uint8_t*)array + size_of_item * index, item_ptr, size_of_item);
  }
}

inline void* lzws_allocate_array(uint_fast8_t size_of_item, size_t length, void* item_ptr, bool item_is_zero, bool item_bytes_are_identical)
{
  size_t size = size_of_item * length;

  if (item_is_zero) {
    return calloc(1, size);
  }

  void* array = malloc(size);
  if (array == NULL) {
    return NULL;
  }

  lzws_fill_array(array, size_of_item, length, item_ptr, item_bytes_are_identical);

  return array;
}

#endif // LZWS_UTILS_H
