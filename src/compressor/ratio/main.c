// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "main.h"

extern inline void lzws_compressor_initialize_ratio(lzws_compressor_ratio_t* ratio_ptr);
extern inline void lzws_compressor_add_source_symbol_to_ratio(lzws_compressor_ratio_t* ratio_ptr);
extern inline void lzws_compressor_add_destination_symbol_to_ratio(lzws_compressor_ratio_t* ratio_ptr);

bool lzws_compressor_need_to_clear_by_ratio(lzws_compressor_ratio_t* ratio_ptr)
{
  // We don't need to clear when destination length equals to zero.
  // Source length won't be zero when destination length is not zero.
  if (ratio_ptr->new_source_length < LZWS_RATIO_SOURCE_CHECKPOINT_GAP || mpz_cmp_ui(ratio_ptr->destination_length, 0) == 0) {
    return false;
  }

  // We need to clear when "destination_length * new_source_length - source_length * new_destination_length" < 0.

  mpz_t destination_and_new_source, source_and_new_destination;
  mpz_inits(destination_and_new_source, source_and_new_destination, NULL);

  mpz_mul_ui(destination_and_new_source, ratio_ptr->destination_length, ratio_ptr->new_source_length);
  mpz_mul_ui(source_and_new_destination, ratio_ptr->source_length, ratio_ptr->new_destination_length);

  bool result = mpz_cmp(destination_and_new_source, source_and_new_destination) < 0;

  mpz_clears(destination_and_new_source, source_and_new_destination, NULL);

  lzws_compressor_clear_ratio(ratio_ptr);

  return result;
}

extern inline void lzws_compressor_clear_ratio(lzws_compressor_ratio_t* ratio_ptr);
extern inline void lzws_compressor_free_ratio(lzws_compressor_ratio_t* ratio_ptr);
