// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "invalid.h"

int main()
{
  lzws_result_t result = lzws_string_test_invalid();
  if (result != 0) {
    return 1;
  }

  return 0;
}
