// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_ALIGNMENT_MAIN_H)
#define LZWS_COMPRESSOR_ALIGNMENT_MAIN_H

#include "../../common.h"

#include "common.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_ALIGNMENT_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_alignment(lzws_compressor_alignment_t* alignment_ptr)
{
  alignment_ptr->unaligned_by_code_bit_length      = LZWS_LOWEST_MAX_CODE_BIT_LENGTH;
  alignment_ptr->unaligned_destination_byte_length = 0;
}

LZWS_INLINE void lzws_compressor_set_last_used_code_bit_length_to_alignment(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t last_used_code_bit_length)
{
  alignment_ptr->unaligned_by_code_bit_length = last_used_code_bit_length;
}

LZWS_INLINE void lzws_compressor_add_destination_byte_length_to_alignment(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t destination_byte_length)
{
  // Destination byte length can be aligned by unaligned by code bit length * 8.

  alignment_ptr->unaligned_destination_byte_length = (alignment_ptr->unaligned_destination_byte_length + destination_byte_length) % alignment_ptr->unaligned_by_code_bit_length;
}

LZWS_INLINE bool lzws_compressor_need_to_write_alignment(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t last_used_code_bit_length)
{
  return alignment_ptr->unaligned_by_code_bit_length != last_used_code_bit_length;
}

#endif // LZWS_COMPRESSOR_ALIGNMENT_MAIN_H
