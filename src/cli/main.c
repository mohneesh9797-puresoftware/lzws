// LZW streaming compressor/decompressor based on LZW AB.
// Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
// Distributed under the BSD Software License (see LICENSE).

#include <getopt.h>
#include <stdlib.h>

#include "../file.h"

static const char* help =
  "Overview: lzws cli tool\n"
  "\n"
  "Usage: lzws [-%s] [< stdin] [> stdout]\n"
  "\n"
  "Options:\n"
  "  --decompress (-d) - enable decompress mode (default mode is compress)\n"
  "  --max-code-bit-length (-b) - set max code bit length (%u-%u) (default value is %u) [compressor only]\n"
  "  --msb (-m) - enable most significant bit (default mode is least significant bit)\n"
  "  --raw (-r) - disable block mode (block mode is enabled by default) [compressor only]\n"
  "  --unaligned-bit-groups (-u) - enable unaligned bit groups (disabled by default)\n"
  "  --quiet (-q) - disable error messages (enabled by default)\n"
  "  --help (-h) - print help\n"
  "\n"
  "Compatibility:\n"
  "  Default options are fully compatible with UNIX compress.\n"
  "  Compressor only options (-b and -r) can be changed without loosing compatibility.\n";

static const char*   short_options  = "db:mruqh";
static struct option long_options[] = {
  {"max-code-bit-length", optional_argument, NULL, 'b'},
  {"decompress", optional_argument, NULL, 'd'},
  {"msb", optional_argument, NULL, 'm'},
  {"help", optional_argument, NULL, 'h'},
  {"quiet", optional_argument, NULL, 'q'},
  {"raw", optional_argument, NULL, 'r'},
  {"unaligned-bit-groups", optional_argument, NULL, 'u'},
  {NULL, 0, NULL, 0}};

static inline void print_help()
{
  fprintf(stderr, help, short_options, LZWS_LOWEST_MAX_CODE_BIT_LENGTH, LZWS_BIGGEST_MAX_CODE_BIT_LENGTH, LZWS_BIGGEST_MAX_CODE_BIT_LENGTH);
}

int main(int argc, char** argv)
{
  bool is_compressor = true;

  uint_fast8_t max_code_bit_length  = LZWS_BIGGEST_MAX_CODE_BIT_LENGTH;
  bool         block_mode           = true;
  bool         msb                  = false;
  bool         unaligned_bit_groups = false;
  bool         quiet                = false;

  int option;

  while ((option = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (option) {
      case 'b':
        max_code_bit_length = atoi(optarg);
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
        unaligned_bit_groups = true;
        break;
      default:
        print_help();
        return 1;
    }
  }

  if (is_compressor) {
    if (lzws_compress_file(stdin, 0, stdout, 0, max_code_bit_length, block_mode, msb, unaligned_bit_groups, quiet) != 0) {
      return 2;
    }
  }
  else {
    if (lzws_decompress_file(stdin, 0, stdout, 0, msb, unaligned_bit_groups, quiet) != 0) {
      return 3;
    }
  }

  return 0;
}
