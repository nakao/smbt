/*
 * BitArray.hpp
 * Copyright (c) 2013 Yasuo Tabei All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE and * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef _BITARRAY_HPP_
#define _BITARRAY_HPP_

#include <iostream>
#include <vector>
#include <stdint.h>

class BitArray {
private:
public:
  BitArray() {
    tail = 0;
  }
  void     setBit(uint64_t bit, uint64_t pos);
  uint64_t getBit(uint64_t pos);
  uint64_t getVal(uint64_t start, uint64_t end);
  void     push(uint64_t bit);
  void     save(std::ostream &os);
  void     load(std::istream &is);
  uint64_t getByte() {
    uint64_t size = (64 * bitarray.size())/8;
    return size;
  }
  uint64_t getSize() {
    return bitarray.size() * BLOCKSIZE;
  }
private:
  static uint64_t BLOCKSIZE;
  static uint64_t clip[64][64];
  std::vector<uint64_t> bitarray;
  uint64_t tail;
};

#endif // _BITARRAY_HPP_
