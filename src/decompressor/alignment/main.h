// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_ALIGNMENT_MAIN_H)
#define LZWS_DECOMPRESSOR_ALIGNMENT_MAIN_H

#include "../../common.h"

#include "common.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_ALIGNMENT_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_decompressor_initialize_alignment(lzws_decompressor_alignment_t* alignment_ptr, uint_fast8_t free_code_bit_length)
{
  alignment_ptr->free_code_bit_length = free_code_bit_length;
  alignment_ptr->source_byte_length   = 0;
}

LZWS_INLINE void lzws_decompressor_add_source_byte_length_to_alignment(lzws_decompressor_alignment_t* alignment_ptr, uint_fast8_t source_byte_length)
{
  // Source byte length can be aligned by free code bit length * 8.
  alignment_ptr->source_byte_length = (alignment_ptr->source_byte_length + source_byte_length) % alignment_ptr->free_code_bit_length;
}

LZWS_INLINE bool lzws_decompressor_need_to_read_alignment(lzws_decompressor_alignment_t* alignment_ptr, uint_fast8_t free_code_bit_length)
{
  return alignment_ptr->free_code_bit_length != free_code_bit_length;
}

LZWS_INLINE bool lzws_decompressor_need_to_read_alignment_byte(lzws_decompressor_alignment_t* alignment_ptr)
{
  return alignment_ptr->source_byte_length != 0;
}

LZWS_INLINE void lzws_decompressor_reset_alignment_after_reading(lzws_decompressor_alignment_t* alignment_ptr, uint_fast8_t free_code_bit_length)
{
  alignment_ptr->free_code_bit_length = free_code_bit_length;
}

#endif // LZWS_DECOMPRESSOR_ALIGNMENT_MAIN_H
