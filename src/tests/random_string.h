// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_TESTS_RANDOM_STRING_H)
#define LZWS_TESTS_RANDOM_STRING_H

#include <stdint.h>
#include <stdlib.h>

void lzws_tests_set_random_string(char* string, size_t string_length);

#endif // LZWS_TESTS_RANDOM_STRING_H
