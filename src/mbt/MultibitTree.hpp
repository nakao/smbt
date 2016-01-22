/*
 * MultibitTree.hpp
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

#ifndef _MULTIBITTREE_HPP_
#define _MULTIBITTREE_HPP_

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <bitset>
#include <cmath>
#include <cstring>
#include <string>
#include <iterator>
#include <fstream>
#include <strstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <stack>
#include <stdint.h>
#include <time.h>
#include <utility> 

namespace MBT{
class Cmp {
public:
  bool operator()(const std::pair<uint64_t, std::vector<uint32_t> > *x, std::pair<uint64_t, std::vector<uint32_t> > *y) {
    return (x->second.size() < y->second.size());
  }
};

struct Node {
  bool                  leaf;
  std::vector<uint32_t> oneCol, zeroCol;
  std::vector<uint32_t> ids;

  Node() {
    oneCol.clear(); zeroCol.clear(); ids.clear();
    leaf = false;
  }

  uint64_t getByte() {
    uint64_t sum = 8;
    sum += oneCol.size() * 2;
    sum += zeroCol.size() * 2;
    sum += ids.size() * 4;
    return sum;
  }
  
  void Save(std::ostream &os) {
    os.write((const char*)&leaf, sizeof(bool));
    {
      size_t size = oneCol.size();
      os.write((const char*)&size, sizeof(size_t));
      os.write((const char*)&oneCol[0], sizeof(uint32_t)*size);
    }
    {
      size_t size = zeroCol.size();
      os.write((const char*)&size, sizeof(size_t));
      os.write((const char*)&zeroCol[0], sizeof(uint32_t)*size);
    }
    {
      size_t size = ids.size();
      os.write((const char*)&size, sizeof(size_t));
      os.write((const char*)&ids[0], sizeof(uint32_t)*size);
    }
  }
  void Read(std::istream &is) {
    is.read((char*)&leaf, sizeof(bool));
    {
      size_t size;
      is.read((char*)&size, sizeof(size_t));
      oneCol.resize(size);
      is.read((char*)&oneCol[0], sizeof(uint32_t)*size);
    }
    {
      size_t size;
      is.read((char*)&size, sizeof(size_t));
      zeroCol.resize(size);
      is.read((char*)&zeroCol[0], sizeof(uint32_t)*size);
    }
    {
      size_t size;
      is.read((char*)&size, sizeof(size_t));
      ids.resize(size);
      is.read((char*)&ids[0], sizeof(uint32_t)*size);
    }
  }
};

struct Tree {
  uint32_t              cardinality;
  std::vector<Node>     nodes;
  std::vector<uint32_t> leftChild;
  std::vector<uint32_t> rightChild;
  Tree() {
    cardinality = 0; nodes.clear(); leftChild.clear(); rightChild.clear();
  }

  uint64_t getByte() {
    uint64_t sum = 8; // for cadinality
    for (size_t i = 0; i < nodes.size(); ++i) {
      sum += nodes[i].getByte();
    }

//    sum += (nodes.size() * 64)/8;
    sum += (leftChild.size() * 64)/8;
    sum += (rightChild.size() * 64)/8;
    return sum;
  }
  
  void Save(std::ostream &os) {
    os.write((const char*)&cardinality, sizeof(uint32_t));
    {
      size_t size = nodes.size();
      os.write((const char*)&size, sizeof(size_t));
      for (size_t i = 0; i < size; ++i)
	nodes[i].Save(os);
    }
    {
      size_t size = leftChild.size();
      os.write((const char*)&size, sizeof(size_t));
      os.write((const char*)&leftChild[0], sizeof(uint32_t)*size);
    }
    {
      size_t size = rightChild.size();
      os.write((const char*)&size, sizeof(size_t));
      os.write((const char*)&rightChild[0], sizeof(uint32_t)*size);
    }
  }
  void Load(std::istream &is) {
    is.read((char*)&cardinality, sizeof(uint32_t));
    {
      size_t size;
      is.read((char*)&size, sizeof(size_t));
      nodes.resize(size);
      for (size_t i = 0; i < size; ++i)
	nodes[i].Read(is);
    }
    {
      size_t size;
      is.read((char*)&size, sizeof(size_t));
      leftChild.resize(size);
      is.read((char*)&leftChild[0], sizeof(uint32_t)*size);
    }
    {
      size_t size;
      is.read((char*)&size, sizeof(size_t));
      rightChild.resize(size);
      is.read((char*)&rightChild[0], sizeof(uint32_t)*size);
    }
  }
};

class MultibitTree {
private:
  void  readFile(std::ifstream &ifs);
  void  readFile(std::ifstream &ifs, std::vector<std::pair<uint64_t, std::vector<uint32_t> >* > &fvs);
  void  readFile(std::ifstream &ifs, std::vector<std::pair<uint64_t, std::vector<uint32_t> >* > &fvs, uint64_t limit);
  float calcEntropy(uint64_t countOne, uint64_t countZero);
  void  buildRangeMultibitTree(uint64_t numOne, uint64_t start, uint64_t end);
  void  buildMultibitTree(const std::vector<std::pair<uint64_t, std::vector<uint32_t> >* > &fvs);
  void  setColumnInfo(std::set<uint64_t> &dims, const std::vector<uint32_t> &ids, Tree &tree, uint32_t cur);
  void  buildRecursive(Tree &tree, uint32_t cur, std::set<uint64_t> dims, std::vector<uint32_t> &ids);
  void  searchQuery(std::vector<uint32_t> &qfv, float similarity, std::vector<std::pair<float, uint64_t> > &res);
  void  searchQueryRecursive(Tree &tree, uint64_t cur, std::vector<uint32_t> &qfv, float similarity, uint32_t oneColNum, uint32_t zeroColNum, std::vector<std::pair<float, uint64_t> > &res);
  void  calcColumnInfo(Tree &tree, uint64_t cur, std::vector<uint32_t> &qfv, uint32_t &oneColNum, uint32_t &zeroColNum);
  bool  pruning(Tree &tree, std::vector<uint32_t> &qfv, float similarity, uint32_t oneColNum, uint32_t zeroColNum);
  void  calcSimilarity(Tree &tree, uint64_t cur, std::vector<uint32_t> &qfv, float similarity, std::vector<std::pair<float, uint64_t> > &res);
  float calcJaccardSim(const std::vector<uint32_t> &ba1, const std::vector<uint32_t> &ba2);
public:
  void  printMemory();
  void  build(const char *fname, uint64_t _minsup);
  void  search(const char *qname, float similarity);
  void  save(std::ostream &os);
  void  load(std::istream &is);

  uint64_t getByte() {
    uint64_t sum = 0;
    for (size_t i = 0; i < fvs.size(); ++i)  {
      sum += 4;
      sum += fvs[i]->second.size() * 4;
    }
    return sum;
  }
private:
  uint64_t                                                   minsup;
  std::vector<std::pair<uint64_t, std::vector<uint32_t> >* > fvs;
  std::vector<Tree>                                          trees;
  uint64_t                                                   dim;
  uint32_t total;
  uint32_t minitem, maxitem;
};
}
#endif // _MULTIBITTREE_HPP_
