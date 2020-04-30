// HTTP old generation (v0.9, v1.0, v1.1) C library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(PRINT_H)
#define PRINT_H

#include <stdbool.h>
#include <stdio.h>

#define PREFIX "  "
#define TERMINATOR ",\n"

#define PRINT(string) fputs(string, stdout)

#define INITIALIZE_SPACERS() bool is_first_spacer = true;

#define PRINT_SPACER()       \
  if (is_first_spacer) {     \
    PRINT(PREFIX);           \
    is_first_spacer = false; \
  }                          \
  else {                     \
    PRINT(TERMINATOR);       \
    PRINT(PREFIX);           \
  }

#endif // PRINT_H
