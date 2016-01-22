/*
 * SucSet.hpp
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
#include <stdint.h>
#include <vector>

class SucSet: public SucArray {
private:
public:
  SucSet() {
    prevVal = 0;
  }
  void push(uint32_t val) {
    SucArray::push(val - prevVal);
    prevVal = val;
  }
  void getAll(std::vector<uint32_t> &vec) {
    size_t size = getSize();
    vec.resize(size);
    for (size_t p = 0; p < size; ++p) {
      if (p == 0)
	vec[p] = get(p);
      else
	vec[p] = get(p) + vec[p-1];
    }
  }
private:
  uint32_t prevVal;
};
