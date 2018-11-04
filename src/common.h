// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_COMPRESSOR_COMMON_H_
#define LZWS_COMPRESSOR_COMMON_H_

#include <stdint.h>

#include "config.h"

typedef uint8_t lzws_result_t;

// External programs requires 2 byte magic header.
// For example: https://github.com/apache/httpd/blob/trunk/modules/metadata/mod_mime_magic.c#L2060
#define LZWS_MAGIC_HEADER_BYTE_0 0x1f // "\037" in base-8 number system.
#define LZWS_MAGIC_HEADER_BYTE_1 0x9d // "\235" in base-8 number system.

#define LZWS_LOWEST_MAX_BITS 9
#define LZWS_BIGGEST_MAX_BITS 16
#define LZWS_MAX_BITS_MASK 0x1f // "max_bits" are the last bits, biggest value (16) requires 5 bits.

#define LZWS_BLOCK_MODE 0x80

#define LZWS_DICTIONARY_SIZE 256
#define LZWS_UNDEFINED_CODE 0
#define LZWS_CLEAR_CODE LZWS_DICTIONARY_SIZE // Code to flush dictionary and restart decoder.
#define LZWS_PREV_FIRST_CODE LZWS_DICTIONARY_SIZE

// "first_child_codes" has no index offset.
#define LZWS_NEXT_SIBLING_CODES_INDEX_OFFSET LZWS_FIRST_CODE
#define LZWS_SYMBOL_BY_CODES_INDEX_OFFSET LZWS_FIRST_CODE

#endif // LZWS_COMPRESSOR_COMMON_H_
