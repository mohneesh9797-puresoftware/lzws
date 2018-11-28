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
  if (!state->block_mode) {
    return;
  }

  lzws_compressor_ratio_t* ratio = &state->ratio;

  ratio->new_source_length      = 0;
  ratio->new_destination_length = 0;

  mpz_inits(ratio->source_length, ratio->destination_length, NULL);
}

LZWS_INLINE void lzws_compressor_add_source_symbol_to_ratio(lzws_compressor_state_t* state) {
  if (!state->block_mode) {
    return;
  }

  lzws_compressor_ratio_t* ratio = &state->ratio;

  if (lzws_compressor_is_dictionary_full(state)) {
    ratio->new_source_length++;
  } else {
    mpz_add_ui(ratio->source_length, ratio->source_length, 1);
  }
}

LZWS_INLINE void lzws_compressor_add_destination_symbol_to_ratio(lzws_compressor_state_t* state) {
  if (!state->block_mode) {
    return;
  }

  lzws_compressor_ratio_t* ratio = &state->ratio;

  if (lzws_compressor_is_dictionary_full(state)) {
    ratio->new_destination_length++;
  } else {
    mpz_add_ui(ratio->destination_length, ratio->destination_length, 1);
  }
}

bool lzws_compressor_calculate_need_to_clear_by_ratio(lzws_compressor_ratio_t* ratio);
void lzws_compressor_calculate_clear_ratio(lzws_compressor_ratio_t* ratio);

LZWS_INLINE bool lzws_compressor_need_to_clear_by_ratio(lzws_compressor_state_t* state) {
  if (!state->block_mode || !lzws_compressor_is_dictionary_full(state)) {
    return false;
  }

  lzws_compressor_ratio_t* ratio = &state->ratio;

  if (ratio->new_source_length < LZWS_RATIO_SOURCE_CHECKPOINT_GAP) {
    return false;
  }

  return lzws_compressor_calculate_need_to_clear_by_ratio(ratio);
}

LZWS_INLINE void lzws_compressor_clear_ratio(lzws_compressor_state_t* state) {
  if (!state->block_mode) {
    return;
  }

  lzws_compressor_calculate_clear_ratio(&state->ratio);
}

LZWS_INLINE void lzws_compressor_free_ratio(lzws_compressor_state_t* state) {
  if (!state->block_mode) {
    return;
  }

  lzws_compressor_ratio_t* ratio = &state->ratio;

  mpz_clears(ratio->source_length, ratio->destination_length, NULL);
}

#endif // LZWS_COMPRESSOR_RATIO_MAIN_H
