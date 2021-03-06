// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <stdint.h>
#include <stdlib.h>

#include "print.h"
#include "print_data.h"

#define XSTR(x) STR(x)
#define STR(x) #x

#define MAX_INDEX 16
#define NUMBER_WIDTH 4

#define BYTE_TEMPLATE "0x%0" XSTR(NUMBER_WIDTH) "x"
#define BYTE_LENGTH_PER_LINE 4
#define BYTE_LENGTH_GLUE ", "

int main()
{
  INITIALIZE_SPACERS();

  for (uint8_t index = 0; index <= MAX_INDEX; index++) {
    if (index % BYTE_LENGTH_PER_LINE == 0) {
      PRINT_SPACER();
    }
    else {
      PRINT(BYTE_LENGTH_GLUE);
    }

    printf(BYTE_TEMPLATE, (1 << index) - 1);
  }

  return 0;
}
