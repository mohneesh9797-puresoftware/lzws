LZW streaming compressor/decompressor (LZW + Stream = LZWS) based on LZW AB.
Copyright (c) 2016 David Bryant, 2018+ other authors, all rights reserved (see AUTHORS).
Distributed under the BSD Software License (see LICENSE).

This implementation target is low memory usage.
It could be slow for big files.

Please read `doc/compressor.txt <doc/compressor.txt>`_ and `doc/decompressor.txt <doc/decompressor.txt>`_

Debug build
-----------

    ::

     $ cd build && cmake .. -DCMAKE_VERBOSE_MAKEFILE=1 && \
        make VERBOSE=1 && make test

Features enable/disable
-----------------------

    ::

     $ cmake .. \
        -DLZWS_CLI=0/1 \
        -DLZWS_SHARED=0/1 \
        -DLZWS_STATIC=0/1

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

  ccc-analyzer toolchain is not working.

    ::

     $ sudo ln -s /usr/lib/llvm/6/libexec/ccc-analyzer /usr/local/bin/ccc-analyzer
