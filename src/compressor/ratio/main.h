// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_RATIO_MAIN_H)
#define LZWS_COMPRESSOR_RATIO_MAIN_H

#include "../state.h"

#undef LZWS_INLINE
#if defined(LZWS_COMPRESSOR_RATIO_MAIN_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

LZWS_INLINE void lzws_compressor_initialize_ratio(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->block_mode) {
    return;
  }

  lzws_compressor_ratio_t* ratio_ptr = &state_ptr->ratio;

  ratio_ptr->new_source_length      = 0;
  ratio_ptr->new_destination_length = 0;

  mpz_inits(ratio_ptr->source_length, ratio_ptr->destination_length, NULL);
}

LZWS_INLINE void lzws_compressor_add_source_symbol_to_ratio(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->block_mode) {
    return;
  }

  state_ptr->ratio.new_source_length++;
}

LZWS_INLINE void lzws_compressor_add_destination_symbol_to_ratio(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->block_mode) {
    return;
  }

  state_ptr->ratio.new_destination_length++;
}

bool lzws_compressor_calculate_need_to_clear_by_ratio(lzws_compressor_ratio_t* ratio_ptr);
void lzws_compressor_calculate_clear_ratio(lzws_compressor_ratio_t* ratio_ptr);

LZWS_INLINE bool lzws_compressor_need_to_clear_by_ratio(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->block_mode || !lzws_compressor_is_dictionary_full(state_ptr)) {
    return false;
  }

  lzws_compressor_ratio_t* ratio_ptr = &state_ptr->ratio;
  if (ratio_ptr->new_source_length < LZWS_RATIO_SOURCE_CHECKPOINT_GAP) {
    return false;
  }

  return lzws_compressor_calculate_need_to_clear_by_ratio(ratio_ptr);
}

LZWS_INLINE void lzws_compressor_clear_ratio(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->block_mode) {
    return;
  }

  lzws_compressor_calculate_clear_ratio(&state_ptr->ratio);
}

LZWS_INLINE void lzws_compressor_free_ratio(lzws_compressor_state_t* state_ptr)
{
  if (!state_ptr->block_mode) {
    return;
  }

  lzws_compressor_ratio_t* ratio_ptr = &state_ptr->ratio;

  mpz_clears(ratio_ptr->source_length, ratio_ptr->destination_length, NULL);
}

#endif // LZWS_COMPRESSOR_RATIO_MAIN_H
