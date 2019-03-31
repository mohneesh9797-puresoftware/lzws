// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "combinations.h"

static const bool block_mode_values[] = {true, false};
#define BLOCK_MODE_VALUES_LENGTH 2

static const bool msb_values[] = {true, false};
#define MSB_VALUES_LENGTH 2

static const bool unaligned_bit_group_values[] = {true, false};
#define UNALIGNED_BIT_GROUP_VALUES_LENGTH 2

#define QUIET false

// -- compressor --

static inline lzws_result_t test_compressor(lzws_test_compressor_t function, va_list args, uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups)
{
  lzws_compressor_state_t* state_ptr;

  lzws_result_t result = lzws_compressor_get_initial_state(&state_ptr, max_code_bit_length, block_mode, msb, unaligned_bit_groups, QUIET);
  if (result != 0) {
    return result;
  }

  // We need to copy arguments for each function call.
  va_list args_copy;
  va_copy(args_copy, args);

  function(state_ptr, args_copy);

  lzws_compressor_free_state(state_ptr);

  return 0;
}

static inline lzws_result_t process_compressor_combinations(lzws_test_compressor_t function, va_list args)
{
  for (uint_fast8_t max_code_bit_length = LZWS_LOWEST_MAX_CODE_BIT_LENGTH; max_code_bit_length <= LZWS_BIGGEST_MAX_CODE_BIT_LENGTH; max_code_bit_length++) {
    for (size_t index = 0; index < BLOCK_MODE_VALUES_LENGTH; index++) {
      bool block_mode = block_mode_values[index];

      for (size_t jndex = 0; jndex < MSB_VALUES_LENGTH; jndex++) {
        bool msb = msb_values[jndex];

        for (size_t kndex = 0; kndex < UNALIGNED_BIT_GROUP_VALUES_LENGTH; kndex++) {
          bool unaligned_bit_groups = unaligned_bit_group_values[kndex];

          lzws_result_t result = test_compressor(function, args, max_code_bit_length, block_mode, msb, unaligned_bit_groups);
          if (result != 0) {
            return result;
          }
        }
      }
    }
  }

  return 0;
}

lzws_result_t lzws_test_compressor_combinations(lzws_test_compressor_t function, ...)
{
  va_list args;
  va_start(args, function);

  lzws_result_t result = process_compressor_combinations(function, args);

  va_end(args);

  return result;
}

// -- decompressor --

static inline lzws_result_t test_decompressor(lzws_test_decompressor_t function, va_list args, bool msb, bool unaligned_bit_groups)
{
  lzws_decompressor_state_t* state_ptr;

  lzws_result_t result = lzws_decompressor_get_initial_state(&state_ptr, msb, unaligned_bit_groups, QUIET);
  if (result != 0) {
    return result;
  }

  // We need to copy arguments for each function call.
  va_list args_copy;
  va_copy(args_copy, args);

  function(state_ptr, args_copy);

  lzws_decompressor_free_state(state_ptr);

  return 0;
}

lzws_result_t process_decompressor_combinations(lzws_test_decompressor_t function, va_list args)
{
  for (size_t index = 0; index < MSB_VALUES_LENGTH; index++) {
    bool msb = msb_values[index];

    for (size_t jndex = 0; jndex < UNALIGNED_BIT_GROUP_VALUES_LENGTH; jndex++) {
      bool unaligned_bit_groups = unaligned_bit_group_values[jndex];

      lzws_result_t result = test_decompressor(function, args, msb, unaligned_bit_groups);
      if (result != 0) {
        return result;
      }
    }
  }

  return 0;
}

lzws_result_t lzws_test_decompressor_combinations(lzws_test_decompressor_t function, ...)
{
  va_list args;
  va_start(args, function);

  lzws_result_t result = process_decompressor_combinations(function, args);

  va_end(args);

  return result;
}

// -- compressor and decompressor --

static inline lzws_result_t test_combination_with_compressor_and_decompressor(lzws_compressor_state_t* compressor_state_ptr, va_list args)
{
  lzws_decompressor_state_t*              decompressor_state_ptr = va_arg(args, lzws_decompressor_state_t*);
  lzws_test_compressor_and_decompressor_t function               = va_arg(args, lzws_test_compressor_and_decompressor_t);
  va_list*                                function_args          = va_arg(args, va_list*);

  // We need to copy arguments for each function call.
  va_list function_args_copy;
  va_copy(function_args_copy, *function_args);

  return function(compressor_state_ptr, decompressor_state_ptr, function_args_copy);
}

static inline lzws_result_t test_compressor_combinations_with_decompressor(lzws_decompressor_state_t* decompressor_state_ptr, va_list args)
{
  lzws_test_compressor_and_decompressor_t function      = va_arg(args, lzws_test_compressor_and_decompressor_t);
  va_list*                                function_args = va_arg(args, va_list*);

  return lzws_test_compressor_combinations(test_combination_with_compressor_and_decompressor, decompressor_state_ptr, function, function_args);
}

lzws_result_t lzws_test_compressor_and_decompressor_combinations(lzws_test_compressor_and_decompressor_t function, ...)
{
  va_list function_args;
  va_start(function_args, function);

  lzws_result_t result = lzws_test_decompressor_combinations(test_compressor_combinations_with_decompressor, function, &function_args);

  va_end(function_args);

  return result;
}

// -- compatible compressor and decompressor --

static inline lzws_result_t test_compatible_compressor_and_decompressor_combination(lzws_compressor_state_t* compressor_state_ptr, lzws_decompressor_state_t* decompressor_state_ptr, va_list args)
{
  if (
    compressor_state_ptr->block_mode != decompressor_state_ptr->block_mode ||
    compressor_state_ptr->msb != decompressor_state_ptr->msb ||
    compressor_state_ptr->unaligned_bit_groups != decompressor_state_ptr->unaligned_bit_groups) {
    return 0;
  }

  lzws_test_compressor_and_decompressor_t function      = va_arg(args, lzws_test_compressor_and_decompressor_t);
  va_list*                                function_args = va_arg(args, va_list*);

  // We need to copy arguments for each function call.
  va_list function_args_copy;
  va_copy(function_args_copy, *function_args);

  return function(compressor_state_ptr, decompressor_state_ptr, function_args_copy);
}

lzws_result_t lzws_test_compatible_compressor_and_decompressor_combinations(lzws_test_compressor_and_decompressor_t function, ...)
{
  va_list function_args;
  va_start(function_args, function);

  lzws_result_t result = lzws_test_compressor_and_decompressor_combinations(test_compatible_compressor_and_decompressor_combination, function, &function_args);

  va_end(function_args);

  return result;
}
