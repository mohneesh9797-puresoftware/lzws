// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "string.h"

#include "buffer.h"
#include "compressor/common.h"
#include "compressor/main.h"
#include "compressor/state.h"
#include "decompressor/common.h"
#include "decompressor/main.h"
#include "decompressor/state.h"
#include "log.h"

// -- buffer --

static inline lzws_result_t increase_destination_buffer(
  uint8_t** destination_ptr, size_t destination_length,
  size_t* remaining_destination_buffer_length_ptr, size_t destination_buffer_length,
  bool quiet)
{
  if (*remaining_destination_buffer_length_ptr == destination_buffer_length) {
    // We want to write more data at once, than buffer has.
    if (!quiet) {
      LZWS_LOG_ERROR("not enough destination buffer, length: %zu", destination_buffer_length);
    }

    return LZWS_STRING_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  lzws_result_t result = lzws_resize_buffer(destination_ptr, destination_length + destination_buffer_length, quiet);
  if (result != 0) {
    return LZWS_STRING_ALLOCATE_FAILED;
  }

  *remaining_destination_buffer_length_ptr = destination_buffer_length;

  return 0;
}

// -- compress --

static inline lzws_result_t compress_data(
  lzws_compressor_state_t* state_ptr,
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool quiet)
{
  lzws_result_t result;

  size_t remaining_destination_buffer_length = destination_buffer_length;

  bool is_finished = false;

  while (true) {
    uint8_t* remaining_destination_buffer             = *destination_ptr + *destination_length_ptr;
    size_t   prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    if (is_finished) {
      result = lzws_finish_compressor(
        state_ptr,
        &remaining_destination_buffer, &remaining_destination_buffer_length);
    }
    else {
      result = lzws_compress(
        state_ptr,
        &source, &source_length,
        &remaining_destination_buffer, &remaining_destination_buffer_length);
    }

    if (
      result != 0 &&
      result != LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION) {
      return LZWS_STRING_COMPRESSOR_UNEXPECTED_ERROR;
    }

    *destination_length_ptr += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (result == LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION) {
      result = increase_destination_buffer(
        destination_ptr, *destination_length_ptr,
        &remaining_destination_buffer_length, destination_buffer_length,
        quiet);

      if (result != 0) {
        return result;
      }

      continue;
    }

    if (is_finished) {
      break;
    }

    is_finished = true;
  }

  result = lzws_resize_buffer(destination_ptr, *destination_length_ptr, quiet);
  if (result != 0) {
    return LZWS_STRING_ALLOCATE_FAILED;
  }

  return 0;
}

lzws_result_t lzws_compress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool without_magic_header, uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups, bool quiet)
{
  lzws_compressor_state_t* state_ptr;

  lzws_result_t result = lzws_compressor_get_initial_state(
    &state_ptr,
    without_magic_header, max_code_bit_length, block_mode, msb, unaligned_bit_groups, quiet);

  if (result != 0) {
    switch (result) {
      case LZWS_COMPRESSOR_ALLOCATE_FAILED:
        return LZWS_STRING_ALLOCATE_FAILED;
      case LZWS_COMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH:
        return LZWS_STRING_VALIDATE_FAILED;
      default:
        return LZWS_STRING_COMPRESSOR_UNEXPECTED_ERROR;
    }
  }

  uint8_t* destination_buffer;

  result = lzws_create_buffer_for_compressor(&destination_buffer, &destination_buffer_length, quiet);
  if (result != 0) {
    lzws_compressor_free_state(state_ptr);

    return LZWS_STRING_ALLOCATE_FAILED;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = 0;

  result = compress_data(
    state_ptr,
    source, source_length,
    destination_ptr, destination_length_ptr, destination_buffer_length,
    quiet);

  lzws_compressor_free_state(state_ptr);

  if (result != 0) {
    free(*destination_ptr);

    return result;
  }

  return 0;
}

// -- decompress --

static inline lzws_result_t decompress_data(
  lzws_decompressor_state_t* state_ptr,
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool quiet)
{
  lzws_result_t result;

  size_t remaining_destination_buffer_length = destination_buffer_length;

  while (true) {
    uint8_t* remaining_destination_buffer             = *destination_ptr + *destination_length_ptr;
    size_t   prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    result = lzws_decompress(
      state_ptr,
      &source, &source_length,
      &remaining_destination_buffer, &remaining_destination_buffer_length);

    if (
      result != 0 &&
      result != LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION) {
      switch (result) {
        case LZWS_DECOMPRESSOR_INVALID_MAGIC_HEADER:
        case LZWS_DECOMPRESSOR_INVALID_MAX_CODE_BIT_LENGTH:
          return LZWS_STRING_VALIDATE_FAILED;
        case LZWS_DECOMPRESSOR_CORRUPTED_SOURCE:
          return LZWS_STRING_DECOMPRESSOR_CORRUPTED_SOURCE;
        default:
          return LZWS_STRING_DECOMPRESSOR_UNEXPECTED_ERROR;
      }
    }

    *destination_length_ptr += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (result == LZWS_DECOMPRESSOR_NEEDS_MORE_DESTINATION) {
      result = increase_destination_buffer(
        destination_ptr, *destination_length_ptr,
        &remaining_destination_buffer_length, destination_buffer_length,
        quiet);

      if (result != 0) {
        return result;
      }

      continue;
    }

    break;
  }

  result = lzws_resize_buffer(destination_ptr, *destination_length_ptr, quiet);
  if (result != 0) {
    return LZWS_STRING_ALLOCATE_FAILED;
  }

  return 0;
}

lzws_result_t lzws_decompress_string(
  uint8_t* source, size_t source_length,
  uint8_t** destination_ptr, size_t* destination_length_ptr, size_t destination_buffer_length,
  bool without_magic_header, bool msb, bool unaligned_bit_groups, bool quiet)
{
  lzws_decompressor_state_t* state_ptr;

  lzws_result_t result = lzws_decompressor_get_initial_state(
    &state_ptr,
    without_magic_header, msb, unaligned_bit_groups, quiet);

  if (result != 0) {
    switch (result) {
      case LZWS_DECOMPRESSOR_ALLOCATE_FAILED:
        return LZWS_STRING_ALLOCATE_FAILED;
      default:
        return LZWS_STRING_DECOMPRESSOR_UNEXPECTED_ERROR;
    }
  }

  uint8_t* destination_buffer;

  result = lzws_create_buffer_for_decompressor(&destination_buffer, &destination_buffer_length, quiet);
  if (result != 0) {
    lzws_decompressor_free_state(state_ptr);

    return LZWS_STRING_ALLOCATE_FAILED;
  }

  *destination_ptr        = destination_buffer;
  *destination_length_ptr = 0;

  result = decompress_data(
    state_ptr,
    source, source_length,
    destination_ptr, destination_length_ptr, destination_buffer_length,
    quiet);

  lzws_decompressor_free_state(state_ptr);

  if (result != 0) {
    free(*destination_ptr);

    return result;
  }

  return 0;
}
