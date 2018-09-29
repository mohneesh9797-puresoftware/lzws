////////////////////////////////////////////////////////////////////////////
//                            **** LZW-AB ****                            //
//               Adjusted Binary LZW Compressor/Decompressor              //
//                     Copyright (c) 2016 David Bryant                    //
//                           All Rights Reserved                          //
//      Distributed under the BSD Software License (see LICENSE)          //
////////////////////////////////////////////////////////////////////////////

This is an implementation of the Lempel-Ziv-Welch general-purpose data compression algorithm.
It is targeted at embedded applications that require high speed compression
or decompression facilities where lots of RAM for large dictionaries might not be available.
I have used this in several projects for storing compressed firmware images,
and once I even coded the decompressor in Z-80 assembly language for speed!
Depending on the maximum symbol size selected, the implementation can require
from 1024 to 15360 bytes of RAM for decoding (and slightly more for encoding).

The symbols are stored in adjusted binary, which provides considerably better compression performance
(with virtually no speed penalty) compared to the fixed sizes nominally used.
To ensure good performance on data with varying characteristics (like executable images)
the encoder resets as soon as the dictionary is full.
Also, worst-case performance is limited to about 8% inflation
by catching poor performance and forcing an early reset before longer symbols are sent.

LZW-AB consists of two standard C files (the library and a command-line demo using pipes)
and builds with a single command on most platforms.
It has been designed with maximum portability in mind
and should work correctly on big-endian as well as little-endian machines.

Debug build
-----------

    ::

     $ cd build && cmake .. -DCMAKE_VERBOSE_MAKEFILE=1 && \
        make VERBOSE=1 && make test

Features enable/disable
-----------------------

    ::

     $ cmake .. \
        -DLZW_AB_SHARED=0/1 \
        -DLZW_AB_STATIC=0/1

Test toolchains
-------------------

    ::

     $ cmake .. -DCMAKE_TOOLCHAIN_FILE="../cmake/toolchains/clang/leak-sanitizer.cmake" \
       make && make test
     $ ../cmake/scripts/test-all-toolchains.sh

DEB and RPM release
-------------------

    ::

     $ cmake .. -DCMAKE_BUILD_TYPE=RELEASE && \
        make && make test && make package

Known issues
------------

    ::

     $ sudo ln -s /usr/lib/llvm/6/libexec/ccc-analyzer /usr/local/bin/ccc-analyzer
