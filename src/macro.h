// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_MACRO_H)
#define LZWS_MACRO_H

#if defined(__GNUC__)
#define LZWS_UNUSED(x) x __attribute__((__unused__))
#else
#define LZWS_UNUSED(x) x
#endif

#endif // LZWS_MACRO_H
