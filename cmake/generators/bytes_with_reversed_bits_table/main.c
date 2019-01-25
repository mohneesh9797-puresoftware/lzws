#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE_LENGTH_PER_LINE 16
#define BYTE_LENGTH_GLUE ", "
#define LINE_PREFIX "  "
#define LINE_TERMINATOR ",\n"

#define PRINT(string) fwrite(string, 1, sizeof(string), stdout)

static inline void print_bytes_with_reversed_bits(uint_fast8_t byte)
{
  uint_fast8_t byte_with_reversed_bits = 0;

  for (uint_fast8_t bit_index = 0; bit_index < 8; bit_index++) {
    uint_fast8_t bit_value = (byte >> bit_index) & 1;
    byte_with_reversed_bits |= bit_value << (7 - bit_index);
  }

  printf("0x%02x", byte_with_reversed_bits);
}

int main()
{
  uint_fast8_t index = 0;

  while (true) {
    if (index != 0) {
      if (index % BYTE_LENGTH_PER_LINE == 0) {
        PRINT(LINE_TERMINATOR);
        PRINT(LINE_PREFIX);
      }
      else {
        PRINT(BYTE_LENGTH_GLUE);
      }
    }

    print_bytes_with_reversed_bits(index);

    if (index == 255) {
      break;
    }
    index++;
  }

  return 0;
}
