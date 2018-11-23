// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#ifndef LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H_
#define LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H_

#include "../../../common.h"

typedef struct {
  lzws_code_t* codes;
} lzws_compressor_dictionary_t;

#endif // LZWS_COMPRESSOR_DICTIONARY_TRIE_ON_SPARSE_ARRAY_MAIN_H_
