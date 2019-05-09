// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "codes.h"

int main()
{
  if (lzws_test_valid_codes() != 0) {
    return 1;
  }

  return 0;
}
