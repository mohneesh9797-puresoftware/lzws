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

LZWS_INLINE void lzws_compressor_initialize_ratio(lzws_compressor_state_t* state) {
  lzws_compressor_ratio_t ratio = state->ratio;

  ratio.need_to_clear = false;

  ratio.new_source_length      = 0;
  ratio.new_destination_length = 0;

  mpz_inits(ratio.source_length, ratio.destination_length, NULL);
}

void lzws_compressor_calculate_clear_for_ratio(lzws_compressor_ratio_t* ratio);

LZWS_INLINE void lzws_compressor_add_source_symbol_to_ratio(lzws_compressor_state_t* state) {
  lzws_compressor_ratio_t ratio = state->ratio;

  ratio.new_source_length++;
  if (ratio.new_source_length != LZWS_RATIO_CHECKPOINT_GAP) {
    return;
  }

  lzws_compressor_calculate_clear_for_ratio(&ratio);
}

LZWS_INLINE void lzws_compressor_add_destination_symbol_to_ratio(lzws_compressor_state_t* state) {
  state->ratio.new_destination_length++;
}

LZWS_INLINE bool lzws_compressor_need_to_clear_by_ratio(lzws_compressor_state_t* state) {
  return state->ratio.need_to_clear;
}

LZWS_INLINE void lzws_compressor_free_ratio(lzws_compressor_state_t* state) {
  lzws_compressor_ratio_t ratio = state->ratio;

  mpz_clears(ratio.source_length, ratio.destination_length, NULL);
}

#endif // LZWS_COMPRESSOR_RATIO_MAIN_H
