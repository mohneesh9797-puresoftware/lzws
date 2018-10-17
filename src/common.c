// LZW streaming compressor/decompressor based on well documented LZW AB
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved
// Distributed under the BSD Software License (see LICENSE)

#include "common.h"

const uint16_t LZW_MAGIC_HEADER = 0x1f9d; // "\037\235"
