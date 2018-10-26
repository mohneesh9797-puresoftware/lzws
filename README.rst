LZW streaming compressor/decompressor based on LZW AB.
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
        -DLZW_AB_CLI=0/1 \
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
