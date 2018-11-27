// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#if !defined(LZWS_COMPRESSOR_DICTIONARY_TYPE_H)
#define LZWS_COMPRESSOR_DICTIONARY_TYPE_H

#include "../../config.h"

#if defined(LZWS_DICTIONARY_TRIE_ON_LINKED_LIST)
#include "trie-on-linked-list/type.h"
#elif defined(LZWS_DICTIONARY_TRIE_ON_SPARSE_ARRAY)
#include "trie-on-sparse-array/type.h"
#endif

#endif // LZWS_COMPRESSOR_DICTIONARY_TYPE_H
