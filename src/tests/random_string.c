// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <time.h>

#include "random_string.h"

void lzws_tests_set_random_string(char* string, size_t string_length)
{
  uint value = time(NULL);

  for (size_t index = 0; index < string_length - 1; index++) {
    value         = value * 1103515245 + 12345;
    string[index] = (value / 65536) % 256;
  }

  string[string_length - 1] = '\0';
}
