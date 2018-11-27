// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#define LZWS_COMPRESSOR_RATIO_MAIN_C

#include "main.h"

void lzws_compressor_calculate_clear_for_ratio(lzws_compressor_ratio_t* ratio) {
  // We need to clear when "destination_length" * "new_source_length" - "source_length" * "new_destination_length" < 0.

  mpz_t destination_and_new_source, source_and_new_destination;
  mpz_inits(destination_and_new_source, source_and_new_destination, NULL);

  mpz_mul_ui(destination_and_new_source, ratio->destination_length, ratio->new_source_length);
  mpz_mul_ui(source_and_new_destination, ratio->source_length, ratio->new_destination_length);

  ratio->need_to_clear = mpz_cmp(destination_and_new_source, source_and_new_destination) < 0;

  mpz_clears(destination_and_new_source, source_and_new_destination, NULL);

  // We need to reset lengths in ratio.

  mpz_add_ui(ratio->source_length, ratio->source_length, ratio->new_source_length);
  ratio->new_source_length = 0;

  mpz_add_ui(ratio->destination_length, ratio->destination_length, ratio->new_destination_length);
  ratio->new_destination_length = 0;
}
