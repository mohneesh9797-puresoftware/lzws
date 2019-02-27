// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include "combinations.h"

#define BLOCK_MODE_VALUES_LENGTH 2
static const bool block_mode_values[] = {true, false};

#define MSB_VALUES_LENGTH 2
static const bool msb_values[] = {true, false};

#define UNALIGNED_BIT_GROUP_VALUES_LENGTH 2
static const bool unaligned_bit_group_values[] = {true, false};

#define QUIET false

static inline lzws_result_t test_compressor(uint_fast8_t max_code_bit_length, bool block_mode, bool msb, bool unaligned_bit_groups, lzws_test_compressor_t* function_ptr, va_list args)
{
  lzws_compressor_state_t* state_ptr;

  lzws_result_t result = lzws_compressor_get_initial_state(&state_ptr, max_code_bit_length, block_mode, msb, unaligned_bit_groups, QUIET);
  if (result != 0) {
    return result;
  }

  (*function_ptr)(state_ptr, args);

  lzws_compressor_free_state(state_ptr);

  return 0;
}

lzws_result_t lzws_test_compressor_combinations(lzws_test_compressor_t* function_ptr, ...)
{
  va_list args;
  va_start(args, function_ptr);

  for (uint_fast8_t max_code_bit_length = LZWS_LOWEST_MAX_CODE_BIT_LENGTH; max_code_bit_length <= LZWS_BIGGEST_MAX_CODE_BIT_LENGTH; max_code_bit_length++) {
    for (size_t index = 0; index < BLOCK_MODE_VALUES_LENGTH; index++) {
      bool block_mode = block_mode_values[index];

      for (size_t jndex = 0; jndex < MSB_VALUES_LENGTH; jndex++) {
        bool msb = msb_values[jndex];

        for (size_t kndex = 0; kndex < UNALIGNED_BIT_GROUP_VALUES_LENGTH; kndex++) {
          bool unaligned_bit_groups = unaligned_bit_group_values[kndex];

          lzws_result_t result = test_compressor(max_code_bit_length, block_mode, msb, unaligned_bit_groups, function_ptr, args);
          if (result != 0) {
            va_end(args);

            return result;
          }
        }
      }
    }
  }

  va_end(args);

  return 0;
}

static inline lzws_result_t test_decompressor(bool msb, bool unaligned_bit_groups, lzws_test_decompressor_t* function_ptr, va_list args)
{
  lzws_decompressor_state_t* state_ptr;

  lzws_result_t result = lzws_decompressor_get_initial_state(&state_ptr, msb, unaligned_bit_groups, QUIET);
  if (result != 0) {
    return result;
  }

  (*function_ptr)(state_ptr, args);

  lzws_decompressor_free_state(state_ptr);

  return 0;
}

lzws_result_t lzws_test_decompressor_combinations(lzws_test_decompressor_t* function_ptr, ...)
{
  va_list args;
  va_start(args, function_ptr);

  for (size_t index = 0; index < MSB_VALUES_LENGTH; index++) {
    bool msb = msb_values[index];

    for (size_t jndex = 0; jndex < UNALIGNED_BIT_GROUP_VALUES_LENGTH; jndex++) {
      bool unaligned_bit_groups = unaligned_bit_group_values[jndex];

      lzws_result_t result = test_decompressor(msb, unaligned_bit_groups, function_ptr, args);
      if (result != 0) {
        va_end(args);

        return result;
      }
    }
  }

  va_end(args);

  return 0;
}
