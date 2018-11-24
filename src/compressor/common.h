// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_COMMON_H)
#define LZWS_COMPRESSOR_COMMON_H

// Possible results:
enum {
  LZWS_COMPRESSOR_ALLOCATE_FAILED = 1,
  LZWS_COMPRESSOR_INVALID_MAX_CODE_BITS,
  LZWS_COMPRESSOR_NEEDS_MORE_SOURCE,
  LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION,
  LZWS_COMPRESSOR_UNKNOWN_STATUS
};

#endif // LZWS_COMPRESSOR_COMMON_H
