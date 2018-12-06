#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "../file.h"

static const char* help =
    "Overview: LZWS cli tool\n"
    "\n"
    "Usage: lzws-cli [-%s] [< stdin] [> stdout]\n"
    "\n"
    "Options:\n"
    "  -b = set max code bits (%u-%u) (%u by default) [compressor only]\n"
    "  -d = decompress (compress by default)\n"
    "  -h = print help\n"
    "  -m = enable most significant bit (least significant bit used by default)\n"
    "  -r = raw mode, disable block mode (enabled by default) [compressor only]\n";

static const char* options = "b:dhmr";

static inline void print_help() {
  fprintf(stderr, help, options, LZWS_LOWEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS);
}

int main(int argc, char** argv) {
  int option;

  bool is_compressor = true;

  uint8_t max_code_bits = LZWS_BIGGEST_MAX_CODE_BITS;
  bool    block_mode    = true;
  bool    msb           = false;

  while ((option = getopt(argc, argv, options)) != -1) {
    switch (option) {
      case 'b':
        max_code_bits = atoi(optarg);
        break;
      case 'd':
        is_compressor = false;
        break;
      case 'm':
        msb = true;
        break;
      case 'r':
        block_mode = false;
        break;
      default:
        print_help();
        return 1;
    }
  }

  if (is_compressor) {
    if (lzws_file_compress(stdin, 0, stdout, 0, max_code_bits, block_mode, msb) != 0) {
      return 2;
    }
  }

  return 0;
}
