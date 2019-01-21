// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <getopt.h>

#include "../file.h"

static const char* help =
  "Overview: LZWS cli tool\n"
  "\n"
  "Usage: lzws-cli [-%s] [< stdin] [> stdout]\n"
  "\n"
  "Options:\n"
  "  --max-code-bits (-b) - set max code bits (%u-%u) (%u by default) [compressor only]\n"
  "  --decompress (-d) - decompress (compress by default)\n"
  "  --msb (-m) - enable most significant bit (least significant bit used by default)\n"
  "  --help (-h) - print help\n"
  "  --quiet (-q) - do not print error messages (disabled by default)\n"
  "  --raw (-r) - raw mode, disable block mode (enabled by default) [compressor only]\n"
  "  --unaligned (-u) - unaligned bit groups (incompatible with original UNIX compress) (disabled by default)\n";

static const char*   short_options  = "b:dmhqru";
static struct option long_options[] = {
  {"max-code-bits", optional_argument, NULL, 'b'},
  {"decompress", optional_argument, NULL, 'd'},
  {"msb", optional_argument, NULL, 'm'},
  {"help", optional_argument, NULL, 'h'},
  {"quiet", optional_argument, NULL, 'q'},
  {"raw", optional_argument, NULL, 'r'},
  {"unaligned", optional_argument, NULL, 'u'},
  {NULL, 0, NULL, 0}};

static inline void print_help() {
  fprintf(stderr, help, short_options, LZWS_LOWEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS, LZWS_BIGGEST_MAX_CODE_BITS);
}

int main(int argc, char** argv) {
  bool is_compressor = true;

  uint_fast8_t max_code_bits = LZWS_BIGGEST_MAX_CODE_BITS;
  bool         block_mode    = true;
  bool         msb           = false;
  bool         quiet         = false;
  bool         unaligned     = false;

  int option;

  while ((option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
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
      case 'q':
        quiet = true;
        break;
      case 'r':
        block_mode = false;
        break;
      case 'u':
        unaligned = true;
        break;
      default:
        print_help();
        return 1;
    }
  }

  if (is_compressor) {
    if (lzws_file_compress(stdin, 0, stdout, 0, max_code_bits, block_mode, msb, quiet, unaligned) != 0) {
      return 2;
    }
  } else {
    if (lzws_file_decompress(stdin, 0, stdout, 0, msb, quiet, unaligned) != 0) {
      return 3;
    }
  }

  return 0;
}
