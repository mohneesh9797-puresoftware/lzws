#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"

static const char* help =
    "Overview: LZWS cli tool\n"
    "\n"
    "Usage: lzws-cli [options] [< stdin] [> stdout]\n"
    "\n"
    "Options:\n"
    "  -b = max code bits for compressor (%u-%u) (%u by default)\n"
    "  -d = decompress (compress by default)\n"
    "  -m = disable block mode (enabled by default)\n";

static const char* options = "b:dmh";

static void print_help() {
  fprintf(stderr, help, LZWS_LOWEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS);
}

int main(int argc, char** argv) {
  int option;

  bool is_compressor = true;

  uint8_t max_code_bits = LZWS_BIGGEST_MAX_CODE_BITS;
  bool    block_mode    = true;

  while ((option = getopt(argc, argv, options)) != -1) {
    switch (option) {
      case 'b':
        max_code_bits = atoi(optarg);
        break;
      case 'd':
        is_compressor = false;
        break;
      case 'm':
        block_mode = false;
        break;
      default:
        print_help();
        return 1;
    }
  }

  if (is_compressor) {
    if (lzws_file_compress(stdin, 0, stdout, 0, max_code_bits, block_mode) != 0) {
      return 2;
    }
  }

  return 0;
}
