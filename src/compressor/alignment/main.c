// LZW streaming compressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "main.h"

extern inline void lzws_compressor_initialize_alignment(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t last_used_code_bit_length);
extern inline void lzws_compressor_add_destination_byte_length_to_alignment(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t destination_byte_length);
extern inline bool lzws_compressor_need_to_write_alignment(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t last_used_code_bit_length);
extern inline bool lzws_compressor_need_to_write_alignment_byte(lzws_compressor_alignment_t* alignment_ptr);
extern inline void lzws_compressor_reset_alignment_after_writing(lzws_compressor_alignment_t* alignment_ptr, uint_fast8_t last_used_code_bit_length);
