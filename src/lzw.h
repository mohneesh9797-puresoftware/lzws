////////////////////////////////////////////////////////////////////////////
//                            **** LZW-AB ****                            //
//               Adjusted Binary LZW Compressor/Decompressor              //
//                     Copyright (c) 2016 David Bryant                    //
//                           All Rights Reserved                          //
//      Distributed under the BSD Software License (see license.txt)      //
////////////////////////////////////////////////////////////////////////////

#ifndef LZW_H_
#define LZW_H_

int lzw_compress(void (*dst)(int), int (*src)(void), int maxbits);
int lzw_decompress(void (*dst)(int), int (*src)(void));

#endif /* LZW_H_ */
