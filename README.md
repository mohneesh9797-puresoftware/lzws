# LZW + Stream = LZWS

LZW streaming compressor/decompressor (LZW + Stream = LZWS) based on LZW AB.

It consists of library and cli tool. Cli tool should be compatible with recent version of [ncompress](https://github.com/vapier/ncompress).

The main goal of the project is to provide streaming interface for lzw compressor. This interface is framework agnostic and could be used in any application.

## Dictionary implementations

* Trie based on linked list (idea from LZW AB). Low memory usage <= 327 KB but slow. Use it for small data < 50 MB.
* Trie based on sparse array. High memory usage <= 33.5 MB and maximum performance. Use it for big data > 50 MB.

You can add your own implementation.

## Getting Started

Project has no deps. You can build it with cmake. It is good to have both gcc and clang for testing.

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

time ./src/cli/lzws-cli < linux-2.6.39.tar > linux-2.6.39.tar.Z
time ./src/cli/lzws-cli -d < linux-2.6.39.tar.Z > linux-2.6.39.tar.new
sha256sum linux-2.6.39.tar
sha256sum linux-2.6.39.tar.new
```

DEB and RPM release:
```sh
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
make
make package
```

## Implementation documentation

* [compressor.txt](doc/compressor.txt)
* [fast_compressor.txt](doc/fast_compressor.txt)
* [decompressor.txt](doc/decompressor.txt)
* [output_compatibility.txt](doc/output_compatibility.txt)

## License

Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
Distributed under the BSD Software License (see LICENSE).
