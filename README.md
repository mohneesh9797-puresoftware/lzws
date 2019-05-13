[![Build Status](https://travis-ci.org/andrew-aladev/lzws.svg?branch=master)](https://travis-ci.org/andrew-aladev/lzws)

# WIP
Man.

# LZW + Stream = LZWS

LZW streaming compressor/decompressor based on LZW AB.

It consists of library and cli tool.
Cli tool should be compatible with original UNIX `compress`, [ncompress](https://github.com/vapier/ncompress) and [gzip unlzw](https://github.com/Distrotech/gzip/blob/distrotech-gzip/unlzw.c).

The main goal of the project is to provide streaming interface for lzw compressor/decompressor. This interface is framework agnostic and can be used in any application.

## Compressor dictionary implementations

* Linked list (idea from LZW AB). It has low memory usage <= 327 KB (16 bit codes). It is slow in general.
* Sparse array. It has high memory usage <= 33.5 MB (16 bit codes). It will be the fastest when block mode disabled or amount of clears is low, otherwise it will be slow.

You can add your own implementation.

## Getting Started

Project depends on [GMP](https://gmplib.org). You can build it with cmake. It is good to have both gcc and clang for testing.

```sh
cd build
cmake ..
make
echo -n "TOBEORNOTTOBEORTOBEORNOT" | ./src/cli/lzws | ./src/cli/lzws -d
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
  -DLZWS_COMPRESSOR_DICTIONARY="linked-list"/"sparse-array"
  -DLZWS_SHARED=0/1
  -DLZWS_STATIC=0/1
  -DLZWS_CLI=0/1
  -DLZWS_TESTS=0/1
```

Testing performance:
```sh
wget "https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.20.3.tar.xz"
tar xf linux-4.20.3.tar.xz
tar cf linux.tar linux-4.20.3

cmake .. -DCMAKE_BUILD_TYPE="RELEASE" -DLZWS_COMPRESSOR_DICTIONARY="linked-list"
make

time ./src/cli/lzws < linux.tar > linux.tar.Z
time ./src/cli/lzws -d < linux.tar.Z > linux.tar.new
sha256sum linux.tar && sha256sum linux.tar.new && sha256sum linux.tar.Z
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
cmake .. -DCMAKE_BUILD_TYPE="RELEASE"
make
make package
```

## Library

```c
// This function is optional.
// Use it to be compatible with with original UNIX compress utility.
lzws_result_t lzws_compressor_write_magic_header(uint8_t** destination_ptr, size_t* destination_length_ptr);

lzws_result_t lzws_compressor_get_initial_state(lzws_compressor_state_t** state_ptr, uint_fast8_t max_code_bit_length, bool block_mode, bool msb);
void          lzws_compressor_free_state(lzws_compressor_state_t* state_ptr);

lzws_result_t lzws_compress(lzws_compressor_state_t* state_ptr, uint8_t** source_ptr, size_t* source_length_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr);

// Use this function when you have no source (received EOF for example).
lzws_result_t lzws_flush_compressor(lzws_compressor_state_t* state_ptr, uint8_t** destination_ptr, size_t* destination_length_ptr);
```

You can see that input was implemented using `uint8_t** source_ptr, size_t* source_length_ptr` and output using `uint8_t** destination_ptr, size_t* destination_length_ptr`.
Functions will read bytes and change both `source_ptr`, `source_length_ptr`, write bytes and change both `destination_ptr` and `destination_length_ptr`.

These methods are framework agnostic. You are free to use it with any files, buffers, sockets, etc.
You can use it with synchronous or asynchronous code.
It won't be hard to implement bindings for ruby, python, etc.

First of all you can use `lzws_compressor_write_magic_header` to be compatible with `compress` and others.
It can return `LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION`.

Than you need to create `lzws_compressor_state_t`. Do not forget to free it later with `lzws_compressor_free_state`.

Than use `lzws_compress` within some read loop.
This method will return `0` when algorithm wants more source (it won't return `LZWS_COMPRESSOR_NEEDS_MORE_SOURCE`).
It can return `LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION`.

Use `lzws_flush_compressor` when you received end of your input (EOF for example).
It can return `LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION`.

`LZWS_COMPRESSOR_NEEDS_MORE_DESTINATION` means that `destination` is filled.
You can write your destination buffer to output.
Than you need to provide new destination (same buffer for example).

PS minimum for `source_length` and `destination_length` is just 2 bytes.
We can recommend to use 32 KB buffers for `linked-list` and 512 KB for `sparse-array`.

Please read [src/file.h](src/file.h) and [src/file.c](src/file.c) for more info.

## Documentation

* [compressor_with_linked_list.txt](doc/compressor_with_linked_list.txt)
* [compressor_with_sparse_array.txt](doc/compressor_with_sparse_array.txt)
* [compressor_ratio.txt](doc/compressor_ratio.txt)
* [decompressor.txt](doc/decompressor.txt)
* [output_compatibility.txt](doc/output_compatibility.txt)

## License

Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
Distributed under the BSD Software License (see LICENSE).

## Why not LZW AB?

It is not compatible with original UNIX `compress` and others.
You can read its code to meet with original idea of "linked-list".
It is well documented.

## Why not ncompress/gzip?

`ncompress` code is not user friendly.
It is full of goto, inliners and it has no comments or any documentation.
There are some issues with overflow and different code bit length.
It is very hard to fix it, you have to know all code history of this project, but it is not available.
It looks almost impossible to modify it for streaming purposes.
It has no tests.

`gzip` uses a bit modified decompressor from ncompress.
It has no compressor.

## Example

There are HTTP servers that supports `Content-Encoding: compress` or `x-compress` via compress or gzip utilities.
See [recent apache 2.0 for example](https://github.com/apache/httpd/blob/trunk/modules/metadata/mod_mime_magic.c#L2055-L2063) uses gzip decompressor.
Apache 1.3.0 and others was using `ncompress`.
You can support this content encoding in your application using `lzws`.
