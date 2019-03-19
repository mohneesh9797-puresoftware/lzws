// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_REMAINDER_H)
#define LZWS_DECOMPRESSOR_REMAINDER_H

#include "state.h"

#undef LZWS_INLINE
#if defined(LZWS_DECOMPRESSOR_REMAINDER_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

// Some UNIX compress implementations writes random bits from uninitialized buffer as remainder bits.
// There is no guarantee that remainder bits will be zeroes at the end of the file or as a part of alignment.
// So in terms of compatibility decompressor have to just ignore remainder bit values.

LZWS_INLINE void lzws_decompressor_clear_remainder(lzws_decompressor_state_t* state_ptr)
{
  state_ptr->remainder            = 0;
  state_ptr->remainder_bit_length = 0;
}

#endif // LZWS_DECOMPRESSOR_REMAINDER_H
