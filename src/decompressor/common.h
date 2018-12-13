// LZW streaming decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_DECOMPRESSOR_COMMON_H)
#define LZWS_DECOMPRESSOR_COMMON_H

// Possible results:
enum {
  LZWS_DECOMPRESSOR_ALLOCATE_FAILED = 1,
  LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER,
  LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BITS,
  LZWS_DECOMPRESSOR_NEEDS_MORE_SOURCE,
  LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION
};

#endif // LZWS_DECOMPRESSOR_COMMON_H
