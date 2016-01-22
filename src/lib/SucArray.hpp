/*
 * SucArray.hpp
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


#ifndef _SUCARRAY_HPP_
#define _SUCARRAY_HPP_

#include <iostream>
#include <fstream>
#include <stdint.h>

#include "../rsdic/lib/RSDicBuilder.hpp"
#include "../rsdic/lib/RSDic.hpp"
#include "./BitArray.hpp"

class SucArray {
private:
  uint64_t getLen(uint64_t val);
public:
  SucArray() {
    size = 0;
  }
  void     build();
  void     push(uint64_t val);
  uint64_t get(uint64_t pos);
  uint64_t getSize() {
    return size;
  }
  uint64_t getByte() {
    uint64_t byte = 4;
    byte += bits.getByte();
    byte += pointers.GetUsageBytes();
    return byte;
  }
  void save(std::ostream &os);
  void load(std::istream &is);
private:
  BitArray            bits;
  rsdic::RSDicBuilder pointersb;
  rsdic::RSDic        pointers;
  uint64_t size;
};

#endif // _SUCARRAY_HPP_
