#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define XSTR(x) STR(x)
#define STR(x) #x

#define MAX_INDEX 16
#define NUMBER_WIDTH 4

#define BYTES_PER_LINE 4
#define BYTES_GLUE ", "
#define LINE_PREFIX "  "
#define LINE_TERMINATOR ",\n"

#define PRINT(string) fwrite(string, 1, sizeof(string), stdout)

int main() {
  for (uint8_t index = 0; index <= MAX_INDEX; index++) {
    if (index != 0) {
      if (index % BYTES_PER_LINE == 0) {
        PRINT(LINE_TERMINATOR);
        PRINT(LINE_PREFIX);
      } else {
        PRINT(BYTES_GLUE);
      }
    }

    printf("0x%0" XSTR(NUMBER_WIDTH) "x", (1 << index) - 1);
  }

  return 0;
}
