// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_LOG_H)
#define LZWS_LOG_H

#include <stdio.h>

#undef LZWS_INLINE
#if defined(LZWS_LOG_C)
#define LZWS_INLINE
#else
#define LZWS_INLINE inline
#endif

#define _LZWS_PRINT_HEADER(target) \
  fprintf(target, "%s %s:%u - ", __func__, __FILE__, __LINE__);

#define _LZWS_PRINT(target, text) \
  _LZWS_PRINT_HEADER(target)      \
  fputs(text, target);            \
  fputs("\n", target);

#define _LZWS_PRINTF(target, ...) \
  _LZWS_PRINT_HEADER(target)      \
  fprintf(target, __VA_ARGS__);   \
  fputs("\n", target);

#define LZWS_PRINT_ERROR(text) _LZWS_PRINT(stderr, text)
#define LZWS_PRINTF_ERROR(...) _LZWS_PRINTF(stderr, __VA_ARGS__)

#endif // LZWS_LOG_H
