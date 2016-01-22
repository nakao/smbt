/*
 * MultibitTree.cpp
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

#include "MultibitTree.hpp"

using namespace std;

namespace MBT{
void MultibitTree::readFile(ifstream &ifs) {
  string line;
  uint32_t val = 0;
  uint64_t lineCount = 0;
  minitem = -1; maxitem = 0;
  while (getline(ifs, line)) {
    if (line.length() == 0)
      continue;
    fvs.resize(fvs.size() + 1);
    fvs[fvs.size() - 1] = new std::pair<uint64_t, std::vector<uint32_t> >();
    uint64_t         &id = (*fvs[fvs.size() - 1]).first;
    vector<uint32_t> &fv = (*fvs[fvs.size() - 1]).second;
    id = lineCount++;
    istringstream is(line);

    while (is >> val) {
      fv.push_back(val);
      if (maxitem < val)
	maxitem = val;
      if (minitem > val)
	minitem = val;
    }
    sort(fv.begin(), fv.end());
  }
}

void MultibitTree::readFile(ifstream &ifs, vector<pair<uint64_t, vector<uint32_t> >* > &fvs, uint64_t limit) {
  string line;
  uint32_t val = 0;
  uint64_t lineCount = 0;
  minitem = -1; maxitem = 0;
  while (getline(ifs, line)) {
    if (line.length() == 0)
      continue;
    fvs.resize(fvs.size() + 1);
    fvs[fvs.size() - 1] = new std::pair<uint64_t, std::vector<uint32_t> >();
    uint64_t         &id = (*fvs[fvs.size() - 1]).first;
    vector<uint32_t> &fv = (*fvs[fvs.size() - 1]).second;
    id = lineCount++;

    istringstream is(line);
    while (is >> val) {
      fv.push_back(val);
      if (maxitem < val)
	maxitem = val;
      if (minitem > val)
	minitem = val;
    }
    sort(fv.begin(), fv.end());
    if (fvs.size() == limit)
      break;
  }
}

void MultibitTree::readFile(ifstream &ifs, vector<pair<uint64_t, vector<uint32_t> >* > &fvs) {
  string line;
  uint32_t val = 0;
  uint64_t lineCount = 0;
  minitem = -1; maxitem = 0;
  while (getline(ifs, line)) {
    if (line.length() == 0)
      continue;
    fvs.resize(fvs.size() + 1);
    fvs[fvs.size() - 1] = new std::pair<uint64_t, std::vector<uint32_t> >();
    uint64_t         &id = (*fvs[fvs.size() - 1]).first;
    vector<uint32_t> &fv = (*fvs[fvs.size() - 1]).second;
    id = lineCount++;
    istringstream is(line);
    while (is >> val) {
      fv.push_back(val);
      if (maxitem < val)
	maxitem = val;
      if (minitem > val)
	minitem = val;
    }
    sort(fv.begin(), fv.end());
  }
}

float MultibitTree::calcEntropy(uint64_t countOne, uint64_t countZero) {
  uint64_t countAll = countOne + countZero;
  float ent = 0.f;
  if (countOne != 0)
    ent += -(float(countOne)/float(countAll)) * log(float(countOne)/float(countAll));
  if (countZero != 0)
    ent += -((float(countZero)/float(countAll)) * log(float(countZero)/float(countAll))) ;
  return ent;
}

void MultibitTree::buildRangeMultibitTree(uint64_t numOne, uint64_t start, uint64_t end) {
  set<uint64_t> dims;
  for (uint32_t i = minitem; i <= maxitem; ++i) 
    dims.insert(i);

  vector<uint32_t> ids;
  for (uint32_t id = start; id <= end; ++id) 
    ids.push_back(id);

  trees.resize(trees.size() + 1);
  Tree &tree = trees[trees.size() - 1];
  tree.cardinality = numOne;
  tree.nodes.resize(1);

  std::set<uint32_t> checker;
  buildRecursive(tree, 0, dims, ids);
}

void MultibitTree::buildRecursive(Tree &tree, uint32_t cur, set<uint64_t> dims, vector<uint32_t> &ids) {
  setColumnInfo(dims, ids, tree, cur);

  if (dims.size() == 0 || ids.size() <= minsup) {
    tree.nodes[cur].ids  = ids;
    tree.nodes[cur].leaf = true;
    return;
  }
  
  vector<uint64_t> count;
  for (size_t i = 0; i < ids.size(); ++i) {
    vector<uint32_t> &fv = fvs[ids[i]]->second;
    for (size_t j = 0; j < fv.size(); ++j) {
      if (count.size() <= fv[j])
	count.resize(fv[j] + 1);
      count[fv[j]]++;
    }
  }

  float    maxEnt = 0.f;
  uint64_t item = *dims.begin();
  for (set<uint64_t>::iterator it = dims.begin(); it != dims.end(); ++it) {
    uint64_t d         = *it;
    float ent = calcEntropy(count[d], ids.size() - count[d]);
    if (ent > maxEnt) {
      maxEnt = ent;
      item   = d;
    }
    if (maxEnt >= 0.95) {
	break;
    }
  }

  vector<uint32_t> childOneIds, childZeroIds;
  for (size_t i = 0; i < ids.size(); ++i) {
    if (binary_search(fvs[ids[i]]->second.begin(), fvs[ids[i]]->second.end(), item)) 
      childOneIds.push_back(ids[i]);
    else
      childZeroIds.push_back(ids[i]);
  }
  dims.erase(item);

  if (childZeroIds.size() > 0) {
    tree.nodes.resize(tree.nodes.size() + 1);
    uint32_t childCur = tree.nodes.size() - 1;
    if (tree.leftChild.size() <= cur)
      tree.leftChild.resize(cur + 1);
    tree.leftChild[cur] = childCur;
    buildRecursive(tree, childCur, dims, childZeroIds);
  }

  if (childOneIds.size() > 0) {
    tree.nodes.resize(tree.nodes.size() + 1);
    uint32_t childCur = tree.nodes.size() - 1;
    if (tree.rightChild.size() <= cur)
      tree.rightChild.resize(cur + 1);
    tree.rightChild[cur] = childCur;
    buildRecursive(tree, childCur, dims, childOneIds);
  }
}

void MultibitTree::buildMultibitTree(const vector<pair<uint64_t,vector<uint32_t> >* > &fvs) {
  uint64_t start = 0;
  uint64_t befNum = fvs[0]->second.size();
  for (size_t i = 1; i < fvs.size(); ++i) {
    uint64_t curNum = fvs[i]->second.size();
    if (befNum != curNum) {
      buildRangeMultibitTree(befNum, start, i - 1);
      start = i;
      befNum = curNum;
    }
  }
  buildRangeMultibitTree(befNum, start, fvs.size() - 1);
}

void MultibitTree::setColumnInfo(set<uint64_t> &dims, const vector<uint32_t> &ids, Tree &tree, uint32_t cur) {
  vector<uint32_t> counter;
  for (size_t i = 0; i < ids.size(); ++i) {
    vector<uint32_t> &fv = fvs[ids[i]]->second;
    for (size_t j = 0; j < fv.size(); ++j) {
      if (fv[j] >= counter.size())
	counter.resize(fv[j] + 1);
      counter[fv[j]]++;
    }
  }
  vector<uint64_t> deleteItems;
  for (set<uint64_t>::iterator it = dims.begin(); it != dims.end(); ++it) {
    uint32_t item = *it;
    if (counter.size() <= item) {
      tree.nodes[cur].zeroCol.push_back(item);
      deleteItems.push_back(item);
    }
    else if (counter[item] == ids.size()) {
      tree.nodes[cur].oneCol.push_back(item);
      deleteItems.push_back(item);
    }
    else if (counter[item] == 0) {
      tree.nodes[cur].zeroCol.push_back(item);
      deleteItems.push_back(item);
    }
  }
  for (size_t i = 0; i < deleteItems.size(); ++i)
    dims.erase(deleteItems[i]);
}

void MultibitTree::build(const char *fname, uint64_t _minsup) {
  minsup = _minsup;
  ifstream ifs(fname);
  if (!ifs) {
    cerr << "cannot open: " << fname << endl;
    exit(1);
  }

  cerr << "reading file:" << fname << endl;
  readFile(ifs, fvs);

  double stime = clock();
  cerr << "sorting" << endl;
  sort(fvs.begin(), fvs.end(), Cmp());

  cerr << "build multibit tree" << endl;
  buildMultibitTree(fvs);
  double etime = clock();
  fprintf(stdout, "construction time (sec):%f\n", (etime - stime)/CLOCKS_PER_SEC);
  printMemory();
  double memory = 0.f;
  for (size_t i = 0; i < fvs.size(); ++i) {
    memory += 32.f/8.f;
    memory += (fvs[i]->second.size() * 32)/8.f;
  }
  fprintf(stdout, "input data size (byte):%f\n", memory);
}

void MultibitTree::searchQuery(vector<uint32_t> &qfv, float similarity, vector<pair<float, uint64_t> > &res) {
  uint64_t query_one_num = qfv.size();
  for (size_t i = 0; i < trees.size(); ++i) {
    Tree &tree = trees[i];
    
    if (similarity * float(tree.cardinality) <= float(query_one_num) && float(query_one_num) <= float(tree.cardinality) / similarity) 
      searchQueryRecursive(tree, 0, qfv, similarity, 0, 0, res);

    if (similarity * float(tree.cardinality) > float(query_one_num))
      break;
  }
}

void MultibitTree::searchQueryRecursive(Tree &tree, uint64_t cur, vector<uint32_t> &qfv, float similarity, uint32_t oneColNum, uint32_t zeroColNum, vector<pair<float, uint64_t> > &res) {
  if (tree.nodes[cur].leaf) {
    calcSimilarity(tree, cur, qfv, similarity, res);
    return;
  }
  calcColumnInfo(tree, cur, qfv, oneColNum, zeroColNum);
  if (pruning(tree, qfv, similarity, oneColNum, zeroColNum)) {
    return;
  }
  
  searchQueryRecursive(tree, tree.leftChild[cur], qfv, similarity, oneColNum, zeroColNum, res);
  searchQueryRecursive(tree, tree.rightChild[cur], qfv, similarity, oneColNum, zeroColNum, res);
}

bool MultibitTree::pruning(Tree &tree, vector<uint32_t> &qfv, float similarity, uint32_t oneColNum, uint32_t zeroColNum) {
  uint64_t cardA    = tree.cardinality;
  uint64_t cardB    = qfv.size();
  uint64_t common   = std::min(cardA - oneColNum, cardB - zeroColNum);
  return (float(common)/float(cardA + cardB - common) < similarity);
}
 
void MultibitTree::calcSimilarity(Tree &tree, uint64_t cur, vector<uint32_t> &qfv, float similarity, vector<pair<float, uint64_t> > &res) {
  vector<uint32_t> &ids = tree.nodes[cur].ids;
  for (size_t id = 0; id < ids.size(); ++id) {
    float sim;
    if ((sim = calcJaccardSim(fvs[ids[id]]->second, qfv)) >= similarity)
      res.push_back(make_pair(sim, fvs[ids[id]]->first));
  }
}

float MultibitTree::calcJaccardSim(const vector<uint32_t> &ba1, const vector<uint32_t> &ba2) {
  uint64_t common = 0;
  size_t i = 0, j = 0;
  while (i < ba1.size() && j < ba2.size()) {
    if      (ba1[i] < ba2[j])   i++;
    else if (ba1[i] > ba2[j])   j++;
    else                        { common++; i++; j++; }
  }
  return float(common)/float(ba1.size() + ba2.size() - common);
}

void MultibitTree::calcColumnInfo(Tree &tree, uint64_t cur, vector<uint32_t> &qfv, uint32_t &oneColNum, uint32_t &zeroColNum) {
  const vector<uint32_t> &oneCol  = tree.nodes[cur].oneCol;
  const vector<uint32_t> &zeroCol = tree.nodes[cur].zeroCol;

  for (size_t i = 0; i < oneCol.size(); ++i) {
    if (binary_search(qfv.begin(), qfv.end(), oneCol[i])  == false)
      oneColNum++;
  }
  for (size_t i = 0; i < zeroCol.size(); ++i) {
    if (binary_search(qfv.begin(), qfv.end(), zeroCol[i]) == true)
      zeroColNum++;
  }
}

void MultibitTree::search(const char *qname, float similarity) {
  ifstream ifs(qname);
  if (!ifs) {
    cerr << "cannot open: " << qname << endl;
    exit(1);
  }
  std::vector<std::pair<uint64_t, vector<uint32_t> >* > qfvs;
  readFile(ifs, qfvs);

  vector<double> times;
  uint64_t totalres = 0;
  for (size_t i = 0; i < qfvs.size(); ++i) {
    double stime = clock();
    uint64_t          qid = qfvs[i]->first;
    vector<uint32_t> &qfv = qfvs[i]->second;
    vector<pair<float, uint64_t> > res;
    searchQuery(qfv, similarity, res);
    totalres += res.size();
    double etime = clock();
    times.push_back((etime - stime)/CLOCKS_PER_SEC);

    cout << "query id:" << qid << " num:" << res.size() << " ";
    for (size_t j = 0; j < res.size(); ++j) 
      cout << res[j].second << ":" << res[j].first << " ";
    cout << endl;
  }
  size_t size = times.size();
  double mean = 0.f;
  for (size_t i = 0; i < size; ++i) {
    mean += times[i];
  }
  mean /= double(size);
  double var = 0.f;
  for (size_t i = 0; i < size; ++i) {
    var += (times[i] - mean) * (times[i] - mean);
  }
  var /= double(size - 1);
  fprintf(stdout, "mean search time (sec):%f\n", mean);
  fprintf(stdout, "var search time (sec):%f\n", var);
  double sum = double(totalres)/double(size);
  fprintf(stdout, "result per query:%f\n", sum);
}

void MultibitTree::printMemory() {
  double memory = 0.f;
  for (size_t i = 0; i < trees.size(); ++i) 
    memory += trees[i].getByte();

  fprintf(stdout, "multibit tree size (byte):%f\n", memory);
}

void MultibitTree::save(ostream &os) {
  os.write((const char*)&minsup, sizeof(uint64_t));
  os.write((const char*)&dim, sizeof(uint64_t));
  {
    size_t size = fvs.size();
    os.write((const char*)&size, sizeof(size_t));
    for (size_t i = 0; i < size; ++i) {
      uint64_t         &id = fvs[i]->first;
      vector<uint32_t> &fv = fvs[i]->second;
      size_t         fsize = fv.size();
      os.write((const char*)&id, sizeof(uint64_t));
      os.write((const char*)&fsize, sizeof(size_t));
      os.write((const char*)&fv[0], sizeof(uint32_t) * fsize);
    }
  }
  {
    size_t size = trees.size();
    os.write((const char*)&size, sizeof(size_t));
    for (size_t i = 0; i < size; ++i)
      trees[i].Save(os);
  }
}

void MultibitTree::load(istream &is)  {
  is.read((char*)&minsup, sizeof(uint64_t));
  is.read((char*)&dim, sizeof(uint64_t));
  {
    size_t size;
    is.read((char*)&size, sizeof(size_t));
    fvs.resize(size);
    for (size_t i = 0; i < size; ++i) {
      fvs[i] = new pair<uint64_t, vector<uint32_t> >;
      uint64_t &id = fvs[i]->first;
      vector<uint32_t> &bt = fvs[i]->second;
      size_t fsize;
      is.read((char*)&id, sizeof(uint64_t));
      is.read((char*)&fsize, sizeof(size_t));
      bt.resize(fsize);
      is.read((char*)&bt[0], sizeof(uint32_t) * fsize);
    }
  }
  {
    size_t size;
    is.read((char*)&size, sizeof(size_t));
    trees.resize(size);
    for (size_t i = 0; i < size; ++i)
      trees[i].Load(is);
  }
}
}
