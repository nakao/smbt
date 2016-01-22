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

#include "SucArray.hpp"

using namespace std;

uint64_t SucArray::getLen(uint64_t val) {
  size_t p;
  for (p = 63; p > 0; --p) {
    if ((val >> p) && 1ULL)
      break;
  }
  return p + 1;
}

void SucArray::build() {
  pointersb.Build(pointers);
}

void SucArray::push(uint64_t val) {
  ++size;
  uint64_t len = getLen(val);
  for (size_t i = 0; i < len; ++i) {
    if ((val >> i) & 1ULL)
      bits.push(1);
    else
      bits.push(0);
  }
  for (size_t i = 1; i < len; ++i) {
    pointersb.PushBack(0);
  }
  pointersb.PushBack(1);
}

uint64_t SucArray::get(uint64_t pos) {
  uint64_t end = pointers.Select(pos, 1);
  uint64_t start;  
  if (pos != 0) 
    start = pointers.Select(pos - 1, 1) + 1;
  else
    start = 0;
  /*
  uint64_t val = 0;
  for (uint64_t i = start; i <= end; ++i) {
    if (bits.getBit(i)) 
      val |= (1ULL << (i - start));
  }
  */
  /*
  if (val != bits.bits.getVal(start, end)) {
  bits.getVal(start, end);
  }
  */
  return bits.getVal(start, end);
}

void SucArray::save(ostream &os) {
  bits.save(os);
  pointers.Save(os);
  os.write((const char*)(&size), sizeof(size));
}

void SucArray::load(istream &is) {
  bits.load(is);
  pointers.Load(is);
  /*
  if (bits.getSize() == 0 && pointers.num() != 0) {
    cerr << bits.getSize() << " " << pointers.num() << endl;
    exit(1);
  }
  */
  is.read((char*)(&size), sizeof(size));
}


