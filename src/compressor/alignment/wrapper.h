// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_ALIGNMENT_WRAPPER_H)
#define LZWS_COMPRESSOR_ALIGNMENT_WRAPPER_H

#include "../state.h"

#include "main.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_ALIGNMENT_WRAPPER_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_alignment_wrapper(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->unaligned_bit_groups) {
    lzws_compressor_initialize_alignment(&state_ptr->alignment, state_ptr->last_used_code_bit_length);
  }
}

LZWS_INLINE void lzws_compressor_add_destination_byte_length_to_alignment_wrapper(lzws_compressor_state_t* state_ptr, uint_fast8_t destination_byte_length)
{
  if (!state_ptr->unaligned_bit_groups) {
    lzws_compressor_add_destination_byte_length_to_alignment(&state_ptr->alignment, destination_byte_length);
  }
}

LZWS_INLINE bool lzws_compressor_need_to_write_alignment_wrapper(lzws_compressor_state_t* state_ptr)
{
  if (state_ptr->unaligned_bit_groups) {
    return false;
  }

  return lzws_compressor_need_to_write_alignment(&state_ptr->alignment, state_ptr->last_used_code_bit_length);
}

lzws_result_t lzws_compressor_write_padding_zeroes_for_alignment_wrapper(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr);

#endif // LZWS_COMPRESSOR_ALIGNMENT_WRAPPER_H
