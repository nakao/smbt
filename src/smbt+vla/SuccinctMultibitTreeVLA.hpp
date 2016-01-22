/*
 * SuccinctMultibitTreeVLA.hpp
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

#ifndef _SUCCINCTMULTIBITTREEVLA_HPP_
#define _SUCCINCTMULTIBITTREEVLA_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <set>
#include <stack>
#include <cmath>
#include <algorithm>
#include <queue>
#include <limits.h>

#include "../rsdic/lib/RSDicBuilder.hpp"
#include "../rsdic/lib/RSDic.hpp"
#include "../lib/SucArray.hpp"
#include "../lib/varByte.hpp"

namespace SMBTVLA{
class Cmp1 {
public:
  bool operator()(const std::pair<uint32_t, std::vector<uint32_t> > *x, std::pair<uint32_t, std::vector<uint32_t> > *y) {
    return (x->second.size() < y->second.size());
  }
};

struct Component {
  uint32_t depth;
  std::vector<uint32_t> oneCols;
  std::vector<uint32_t> zeroCols;
  std::vector<uint32_t> ids;
  bool leaf;
  Component() {
    depth = 0; oneCols.clear(); zeroCols.clear(); ids.clear(); leaf = false;
  }
};

class Cmp2 {
public:
  bool operator()(const Component *x, const Component *y) {
    return (x->depth < y->depth);
  }
};

class Cmp3 {
public:
  bool operator()(const std::pair<uint32_t, std::vector<uint32_t> > *x, const std::pair<uint32_t, std::vector<uint32_t> > *y) {
    const std::vector<uint32_t> &fvx = x->second;
    const std::vector<uint32_t> &fvy = y->second;
    size_t size = std::min(fvx.size(), fvy.size());
    for (size_t i = 0; i < size; ++i) {
      if      (fvx[i] < fvy[i])
	return 1;
      else if (fvx[i] > fvy[i])
	return 0;
    }
    if (fvx.size() < fvy.size())
      return 1;
    else
      return 0;
  }
};

struct Tree {
  uint32_t            cardinality;
  rsdic::RSDicBuilder badb;
  rsdic::RSDic        ba;
  rsdic::RSDicBuilder leavesdb;
  rsdic::RSDic        leaves;
  std::vector<VarByte> oneCols;
  std::vector<VarByte> zeroCols;
  std::vector<VarByte> ids;

  Tree() {
    cardinality = 0; 
  }

  uint64_t getByte() {
    uint64_t sum = 0;
    sum += 4;

    sum += ba.GetUsageBytes();
    sum += leaves.GetUsageBytes();
    for (size_t i = 0; i < oneCols.size(); ++i) 
      sum += oneCols[i].size();
    for (size_t i = 0; i < zeroCols.size(); ++i) 
      sum += zeroCols[i].size();
    for (size_t i = 0; i < ids.size(); ++i) {
      sum += ids[i].size();
    }
    return sum;
  }
  
  uint64_t getFirstChild(uint64_t cur) {
    return ba.Select(ba.Rank(cur+1,1)-1, 0) + 1;
  }

  bool leaf(uint64_t cur) {
    return (bool)leaves.GetBit(cur);
  }

  void getIds(uint64_t cur, std::vector<uint32_t> &vec) {
    ids[leaves.Rank(cur, 1)].decode(vec);
  }
  
  void build(std::vector<Component*> &components) {
    badb.PushBack(1);
    badb.PushBack(0);
    leavesdb.PushBack(0);
    leavesdb.PushBack(0);
    uint64_t pos = 2;
    for (size_t i = 0; i < components.size(); ++i) {
      std::vector<uint32_t> &tmpOneCols  = components[i]->oneCols;
      std::vector<uint32_t> &tmpZeroCols = components[i]->zeroCols;
      std::vector<uint32_t> &tmpIds      = components[i]->ids;
      bool                   leaf        = components[i]->leaf;                

      if (!leaf) {
	oneCols.resize(pos+1);
	oneCols[pos].encode(tmpOneCols);
	/*
	for (size_t i = 0; i < tmpOneCols.size(); ++i) 
	  oneCols[pos].push(tmpOneCols[i]);
	*/
	zeroCols.resize(pos+1) ;
	zeroCols[pos].encode(tmpZeroCols);
	/*
	for (size_t i = 0; i < tmpZeroCols.size(); ++i)
	  zeroCols[pos].push(tmpZeroCols[i]);
	*/
	pos += 3;
	leavesdb.PushBack(0);
	leavesdb.PushBack(0);
	leavesdb.PushBack(0);
	badb.PushBack(1);
	badb.PushBack(1);
	badb.PushBack(0);
      }
      else {
	pos += 1;
	leavesdb.PushBack(1);
	badb.PushBack(0);
	ids.resize(ids.size() + 1);
	ids[ids.size() - 1].encode(tmpIds);
	/*
	for (size_t i = 0; i < tmpIds.size(); ++i)
	  ids[ids.size() - 1].push(tmpIds[i]);
	*/
      }
    }
    badb.Build(ba);
    leavesdb.Build(leaves);
  }

  void save(std::ofstream &os) {
    ba.Save(os);
    leaves.Save(os);
    os.write((const char*)(&cardinality), sizeof(cardinality));
    {
      size_t size = oneCols.size();
      os.write((const char*)(&size), sizeof(size));
      for (size_t i = 0; i < size; ++i) 
	oneCols[i].save(os);
    }
    {
      size_t size = zeroCols.size();
      os.write((const char*)(&size), sizeof(size));
      for (size_t i = 0; i < size; ++i) 
	zeroCols[i].save(os);
    }
    {
      size_t size = ids.size();
      os.write((const char*)(&size), sizeof(size));
      for (size_t i = 0; i < size; ++i) 
	ids[i].save(os);
    }
  }

  void load(std::ifstream &is) {
    ba.Load(is);
    leaves.Load(is);
    is.read((char*)(&cardinality), sizeof(cardinality));
    {
      size_t size;
      is.read((char*)(&size), sizeof(size));
      oneCols.resize(size);
      for (size_t i = 0; i < size; ++i)
	oneCols[i].load(is);
    }
    {
      size_t size;
      is.read((char*)(&size), sizeof(size));
      zeroCols.resize(size);
      for (size_t i = 0; i < size; ++i)  
	zeroCols[i].load(is);
    }
    {
      size_t size;
      is.read((char*)(&size), sizeof(size));
      ids.resize(size);
      for (size_t i = 0; i < size; ++i) 
	ids[i].load(is);
    }
  }
};

struct QueueElem {
  uint32_t left;
  uint32_t right;
  uint32_t depth;
  QueueElem(uint32_t _left, uint32_t _right, uint32_t _depth) {
    left = _left; right = _right; depth = _depth;
  }
  void set(uint32_t _left, uint32_t _right, uint32_t _depth) {
    left = _left; right = _right; depth = _depth;
  }
};

class SuccinctMultibitTreeVLA{
private:
  void     readFile(std::ifstream &ifs, std::vector<std::pair<uint32_t, std::vector<uint32_t> >* > &fvs, uint64_t limit);
  void     readFile(std::ifstream &ifs, std::vector<std::pair<uint32_t, std::vector<uint32_t> >* > &fvs);
  void     buildMultibitTree();
  void     buildMultibitTreeRecursive(uint32_t depth, std::vector<uint32_t> &ids, std::set<uint32_t> items, std::vector<Component*> &components);
  void     buildRangeMultibitTree(uint64_t numOne, uint64_t start, uint64_t end);
  void     buildTrie();
  void     calcSplitItem(std::vector<uint32_t> &ids, std::set<uint32_t> &items, uint32_t &item);
  float    calcEntropy(uint32_t numOne, uint32_t numZero);
  void     calcDominantItems(std::set<uint32_t> &items, std::vector<uint32_t> &ids, std::vector<uint32_t> &oneCols, std::vector<uint32_t> &zeroCols, std::vector<uint32_t> &deleteItems);
  void     splitData(uint32_t item, std::vector<uint32_t> &ids, std::vector<uint32_t> &oneIds, std::vector<uint32_t> &zeroIds);
  void     searchQuery(std::vector<uint32_t> &qfv, std::vector<std::pair<float,  uint32_t> > &res);
  void     searchQueryRecursive(Tree &tree, uint64_t cur, std::vector<uint32_t> &qfv, uint32_t oneColNum, uint32_t zeroColNum, std::vector<std::pair<float,  uint32_t> > &res);
  void     calcColumnInfo(Tree &tree, uint64_t cur, std::vector<uint32_t> &qfv, uint32_t &oneColNum, uint32_t &zeroColNum);
  bool     pruning(Tree &tree, std::vector<uint32_t> &qfv, uint32_t oneColNum, uint32_t zeroColNum);
  void     calcSimilarity(Tree &tree, uint64_t cur, std::vector<uint32_t> &qfv, std::vector<std::pair<float, uint32_t> > &res);
  float    jaccardSim(uint32_t id, const std::vector<uint32_t> &qfv);
  uint64_t getParent(uint64_t pos);
  bool     leaf(uint64_t pos);
  uint64_t getFirstChild(uint64_t pos);
  uint32_t getItem(uint64_t pos);
  void     compressItems(std::vector<uint32_t> &titems);

public:
  void     build(const char *fname, size_t _minsup);
  void     search(const char *qname, float _sim);
  void     save(std::ofstream &os);
  void     load(std::ifstream &is);

  uint64_t getByte() {
    uint64_t sum = 0;
    for (size_t i = 0; i < trees.size(); ++i)
      sum += trees[i].getByte();
    return sum;
  }
  uint64_t getTrieByte() {
    uint64_t sum = 0;
    sum += itemset.size() * sizeof(uint32_t);
    for (size_t i = 0; i < itemset.size(); ++i) 
      sum += itemset[i].second.getByte();
    return sum;
  }

private:
  uint32_t minitem, maxitem;
  std::vector<std::pair<uint32_t, std::vector<uint32_t> >* > fvs;
  size_t minsup;
  float  similarity;
  std::vector<Tree> trees;
  std::vector<std::pair<uint32_t, SucArray> > itemset;
  uint32_t total;
};
}
#endif // _SUCCINCTMULTIBITTREEVLA_HPP_
