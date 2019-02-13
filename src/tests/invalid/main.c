// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "max_code_bit_length.h"

int main()
{
  if (lzws_test_invalid_max_code_bit_length() != 0) {
    return 1;
  }

  return 0;
}
