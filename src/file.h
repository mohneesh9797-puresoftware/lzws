// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_FILE_H_
#define LZWS_FILE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "common.h"

// Possible results:
enum {
  LZWS_FILE_ALLOCATE_FAILED = 1,
  LZWS_FILE_COMPRESSOR_FAILED,
  LZWS_FILE_NOT_ENOUGH_DESTINATION_BUFFER,
  LZWS_FILE_WRITE_FAILED
};

// 32 KB for each buffer is enough.
#define DEFAULT_SOURCE_BUFFER_LENGTH 1 << 15
#define DEFAULT_DESTINATION_BUFFER_LENGTH DEFAULT_SOURCE_BUFFER_LENGTH

// "source_buffer_length" and "destination_buffer_length" can be equal to 0, it will use default values.
lzws_result_t lzws_file_compress(
    FILE*   source_file,
    size_t  source_buffer_length,
    FILE*   destination_file,
    size_t  destination_buffer_length,
    uint8_t max_code_bits,
    bool    block_mode);

#endif // LZWS_FILE_H_
