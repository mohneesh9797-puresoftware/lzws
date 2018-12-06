#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTES_PER_LINE 16
#define BYTES_GLUE ", "
#define LINE_PREFIX "  "
#define LINE_TERMINATOR ",\n"

#define PRINT(string) fwrite(string, 1, sizeof(string), stdout)

static inline void print_reversed_byte(uint8_t byte) {
  uint8_t reversed_byte = 0;

  for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
    uint8_t bit_value = (byte >> bit_index) & 1;
    reversed_byte |= bit_value << (7 - bit_index);
  }

  printf("0x%02x", reversed_byte);
}

int main() {
  uint8_t index = 0;

  while (true) {
    if (index != 0) {
      if (index % BYTES_PER_LINE == 0) {
        PRINT(LINE_TERMINATOR);
        PRINT(LINE_PREFIX);
      } else {
        PRINT(BYTES_GLUE);
      }
    }

    print_reversed_byte(index);

    if (index == 255) {
      break;
    }
    index++;
  }

  return 0;
}
