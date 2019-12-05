// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "utils.h"

extern inline uint_fast8_t  lzws_get_byte_with_reversed_bits(uint_fast8_t byte);
extern inline uint_fast16_t lzws_get_mask_for_last_bits(uint_fast8_t bit_length);
extern inline uint_fast16_t lzws_get_max_value_for_bits(uint_fast8_t bit_length);
extern inline size_t        lzws_floor_bit_length_to_byte_length(size_t bit_length);
extern inline size_t        lzws_ceil_bit_length_to_byte_length(size_t bit_length);
extern inline void          lzws_read_byte(uint_fast8_t* byte_ptr, uint8_t** source_ptr, size_t* source_length_ptr);
extern inline void          lzws_write_byte(uint_fast8_t byte, uint8_t** destination_ptr, size_t* destination_length_ptr);
extern inline void          lzws_fill_array(void* array, size_t size_of_item, size_t length, void* item_ptr, bool item_bytes_are_identical);
extern inline void*         lzws_allocate_array(uint_fast8_t size_of_item, size_t length, void* item_ptr, bool item_is_zero, bool item_bytes_are_identical);
