// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_TESTS_FILE_INVALID_H)
#define LZWS_TESTS_FILE_INVALID_H

#include "../../common.h"

// Possible results:
enum {
  LZWS_FILE_TEST_INVALID_MAX_CODE_BIT_LENGTH = 1
};

lzws_result_t lzws_file_test_invalid();

#endif // LZWS_TESTS_FILE_INVALID_H
