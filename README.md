# LZW + Stream = LZWS

LZW streaming compressor/decompressor (LZW + Stream = LZWS) based on LZW AB.

It consists of library and cli tool.
Cli tool should be compatible with recent version of [ncompress](https://github.com/vapier/ncompress) and [gzip unlzw](https://github.com/Distrotech/gzip/blob/distrotech-gzip/unlzw.c).

The main goal of the project is to provide streaming interface for lzw compressor. This interface is framework agnostic and can be used in any application.

## Dictionary implementations

* Trie based on linked list (idea from LZW AB). Low memory usage <= 327 KB (16 bit codes) but slow. Use it for small data < 50 MB.
* Trie based on sparse array. High memory usage <= 33.5 MB (16 bit codes) and maximum performance. Use it for big data > 50 MB.

You can add your own implementation.

## Getting Started

Project depends on [GMP](https://gmplib.org). You can build it with cmake. It is good to have both gcc and clang for testing.

```sh
cd build
cmake ..
make
echo -n "TOBEORNOTTOBEORTOBEORNOT" | ./src/cli/lzws-cli | ./src/cli/lzws-cli -d
```

Debug build:
```sh
cmake .. -DCMAKE_VERBOSE_MAKEFILE=1
make VERBOSE=1
make test
```

You can enable/disable features:
```sh
cmake .. \
  -DLZWS_DICTIONARY="trie-on-linked-list"/"trie-on-sparse-array"
  -DLZWS_SHARED=0/1
  -DLZWS_STATIC=0/1
  -DLZWS_CLI=0/1
  -DLZWS_TESTS=0/1
```

Testing performance:
```sh
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DLZWS_DICTIONARY="trie-on-sparse-array"
make

wget "https://cdn.kernel.org/pub/linux/kernel/v2.6/linux-2.6.39.tar.xz"
tar xf linux-2.6.39.tar.xz
tar cvf linux-2.6.39.tar linux-2.6.39

time ./src/cli/lzws-cli-static < linux-2.6.39.tar > linux-2.6.39.tar.Z
time ./src/cli/lzws-cli-static -d < linux-2.6.39.tar.Z > linux-2.6.39.tar.new
sha256sum linux-2.6.39.tar
sha256sum linux-2.6.39.tar.new
```

Toolchains:
```sh
cmake .. -DCMAKE_TOOLCHAIN_FILE="../cmake/toolchains/clang/leak-sanitizer.cmake"
make
make test

../cmake/scripts/test-all-toolchains.sh
```

DEB and RPM release:
```sh
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make
make package
```

## Library

```c
// This function is optional.
// Use it to be compatible with with original UNIX compress utility.
lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination, size_t* destination_length);

lzws_result_t lzws_compressor_get_initial_state(lzws_compressor_state_t** state, uint8_t max_code_bits, bool block_mode);
void          lzws_compressor_free_state(lzws_compressor_state_t* state);

lzws_result_t lzws_compress(lzws_compressor_state_t* state, uint8_t** source, size_t* source_length, uint8_t** destination, size_t* destination_length);

// Use this function when you have no source (received EOF for example).
lzws_result_t lzws_flush_compressor(lzws_compressor_state_t* state, uint8_t** destination, size_t* destination_length);
```

You can see that input was implemented using `uint8_t** source, size_t* source_length` and output with `uint8_t** destination, size_t* destination_length`.
Functions will read bytes and change both `source`, `source_length`, write bytes and change both `destination` and `destination_length`.

These methods are framework agnostic. You are free to use it with any files, buffers, sockets, etc.
You can use it with synchronous or asynchronous code.
It won't be hard to implement bindings for ruby, python, etc.

First of all you can use `lzws_compressor_write_magic_header` to be compatible with `ncompress` and others.
It can return `LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION`.

Than you need to create `lzws_compressor_state_t`. Do not forget to free it later with `lzws_compressor_free_state`.

Than use `lzws_compress` within some read loop.
This method will return `0` when algorithm wants more source (it won't return `LZWS_COMPRESSOR_NEEDS_MORE_SOURCE`).
It can return `LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION`.

Use `lzws_flush_compressor` when you received end of your input (EOF for example).
It can return `LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION`.

`LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION` means that `destination` is 100% filled.
You can write your destination buffer to output.
Than you need to provide new destination (same buffer for example).

PS minimum `destination_length` is just 2 bytes, `source_length` is 1 byte.
We can recommend to use 32 KB buffers for `trie-on-linked-list` and 1 MB for `trie-on-sparse-array`.

Please read [src/file.h](src/file.h) and [src/file.c](src/file.c) for more info.

## Documentation

* [compressor.txt](doc/compressor.txt)
* [fast_compressor.txt](doc/fast_compressor.txt)
* [compressor_ratio.txt](doc/compressor_ratio.txt)
* [decompressor.txt](doc/decompressor.txt)
* [output_compatibility.txt](doc/output_compatibility.txt)

## License

Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
Distributed under the BSD Software License (see LICENSE).

## Why not LZW AB?

It is not compatible with `ncompress` and original UNIX `compress`.
You can read its code to meet with original idea of "trie-on-linked-list".
It is well documented.

## Why not ncompress/gzip?

`ncompress` code is not user friendly.
It is full of goto, inliners and it has no comments or any documentation.
I've found here some overflow issues.
It is very hard to modify it for streaming purposes.
It has no tests.

`gzip` uses a bit modified decompressor from ncompress.
It has no compressor.

## Example

There are HTTP servers that supports `Content-Encoding: compress` or `x-compress` via compress or gzip utilities.
See [recent apache 2.0 for example](https://github.com/apache/httpd/blob/trunk/modules/metadata/mod_mime_magic.c#L2055-L2063) uses gzip decompressor.
Apache 1.3.0 and others was using `ncompress`.
You can support this content encoding in your application using lzws.
