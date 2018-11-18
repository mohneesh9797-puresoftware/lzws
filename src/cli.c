#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compressor.h"

static const char* help =
    "Overview: LZWS cli tool\n"
    "\n"
    "Usage: lzws-cli [options] [< stdin] [> stdout]\n"
    "\n"
    "Operation: compression is default, use -d to decompress\n"
    "\n"
    "Options:\n"
    "  -b = max code bits for compressor (%u-%u)\n"
    "  -d = decompress\n";

static const char* options = "b:dh";

static void print_help() {
  fprintf(stderr, help, LZWS_LOWEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS);
}

#define BUFFER_LENGTH 1024

uint8_t source_buffer[BUFFER_LENGTH];
uint8_t destination_buffer[BUFFER_LENGTH];

int main(int argc, char** argv) {
  int     option;
  uint8_t max_code_bits = LZWS_LOWEST_MAX_CODE_BITS;
  bool    is_compressor = true;

  while ((option = getopt(argc, argv, options)) != -1) {
    switch (option) {
      case 'b':
        max_code_bits = atoi(optarg);
        break;
      case 'd':
        is_compressor = false;
        break;
      default:
        print_help();
        return 1;
    }
  }

  uint8_t* destination        = destination_buffer;
  size_t   destination_length = BUFFER_LENGTH;

  lzws_compressor_state_t* state;
  if (is_compressor) {
    if (lzws_compressor_write_magic_header(&destination, &destination_length) != 0 ||
        lzws_get_initial_compressor_state(&state, max_code_bits, true) != 0) {
      return 2;
    }
  }

  uint8_t* source        = source_buffer;
  size_t   source_length = BUFFER_LENGTH;

  while ((source_length = fread(source, 1, source_length, stdin)) != 0) {
    if (is_compressor) {
      if (lzws_compress(state, &source, &source_length, &destination, &destination_length) != LZWS_COMPRESSOR_NEEDS_MORE_SOURCE) {
        return 3;
      }
    }
  }

  if (lzws_flush_compressor(state, &destination, &destination_length) != 0) {
    return 4;
  }

  fwrite(destination_buffer, 1, BUFFER_LENGTH - destination_length, stdout);

  return 0;
}
