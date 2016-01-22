/*
 * SuccinctMultibitTreeTRIE.cpp
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

#include "SuccinctMultibitTreeTRIE.hpp"

using namespace std;

namespace SMBTTRIE{

void SuccinctMultibitTreeTRIE::readFile(ifstream &ifs, vector<pair<uint32_t, vector<uint32_t> >* > &fvs, uint64_t limit) {
  string   line;
  uint32_t item;
  uint32_t id = 0;
  maxitem = 0; minitem = UINT_MAX;
  while (getline(ifs, line)) {
    if (line.size() == 0)
      continue;
    stringstream ss(line);
    fvs.resize(fvs.size() + 1);
    fvs[fvs.size() - 1] = new pair<uint32_t, vector<uint32_t> >;
    fvs[fvs.size() - 1]->first = id++;
    vector<uint32_t> &fv = fvs[fvs.size() - 1]->second;
    while (ss >> item) {
      fv.push_back(item);
      if (maxitem < item)
	maxitem = item;
      if (minitem > item)
	minitem = item;
    }
    sort(fv.begin(), fv.end());
    if (fvs.size() == limit) {
      break;
    }
  }
}

void SuccinctMultibitTreeTRIE::readFile(ifstream &ifs, vector<pair<uint32_t, vector<uint32_t> >* > &fvs) {
  string   line;
  uint32_t item;
  uint32_t id = 0;
  maxitem = 0; minitem = UINT_MAX;
  while (getline(ifs, line)) {
    if (line.size() == 0)
      continue;
    stringstream ss(line);
    fvs.resize(fvs.size() + 1);
    fvs[fvs.size() - 1] = new pair<uint32_t, vector<uint32_t> >;
    fvs[fvs.size() - 1]->first = id++;
    vector<uint32_t> &fv = fvs[fvs.size() - 1]->second;
    while (ss >> item) {
      fv.push_back(item);
      if (maxitem < item)
	maxitem = item;
      if (minitem > item)
	minitem = item;
    }
    sort(fv.begin(), fv.end());
  }
}

void SuccinctMultibitTreeTRIE::buildRangeMultibitTree(uint64_t numOne, uint64_t start, uint64_t end) {
  set<uint32_t> items;
  for (size_t i = minitem; i <= maxitem; ++i) {
    items.insert(i);
  }

  vector<uint32_t> ids;
  for (uint64_t id = start; id <= end; ++id)
    ids.push_back(id);

  vector<Component*> components;
  buildMultibitTreeRecursive(0, ids, items, components);
  stable_sort(components.begin(), components.end(), Cmp2());
  trees.resize(trees.size() + 1);
  trees[trees.size() - 1].cardinality = numOne;
  trees[trees.size() - 1].build(components);

  for (size_t i = 0; i < components.size(); ++i) 
    delete components[i];
}

void SuccinctMultibitTreeTRIE::buildMultibitTree() {
  sort(fvs.begin(), fvs.end(), Cmp1());

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

float SuccinctMultibitTreeTRIE::calcEntropy(uint32_t numOne, uint32_t numZero) {
  uint32_t numAll = numOne + numZero;
  float ent = 0.f;
  if (numOne != 0)
    ent += -((float(numOne)/float(numAll)) * log(float(numOne)/float(numAll)));
  if (numZero != 0) 
    ent += -((float(numZero)/float(numAll)) * log(float(numZero)/float(numAll)));

  return ent;
}

void SuccinctMultibitTreeTRIE::calcSplitItem(vector<uint32_t> &ids, set<uint32_t> &items, uint32_t &item) {
  vector<uint32_t> countOne;
  for (size_t i = 0; i < ids.size(); ++i) {
    vector<uint32_t> &fv = fvs[ids[i]]->second;
    for (size_t j = 0; j < fv.size(); ++j) {
      if (countOne.size() <= fv[j])
	countOne.resize(fv[j] + 1);
      ++countOne[fv[j]];
    }
  }

  float maxEnt = 0;
  item = 0;
  for (set<uint32_t>::iterator it = items.begin(); it != items.end(); ++it) {
    uint32_t numOne, numZero;
    if (countOne.size() <= *it) {
      numOne = 0; numZero = ids.size();
    }
    else {
      numOne = countOne[*it]; numZero = ids.size() - numOne;
    }
    float currentEnt = calcEntropy(numOne, numZero);
    if (maxEnt < currentEnt) {
      maxEnt = currentEnt;
      item = *it;
    }
    if (maxEnt >= 0.95) 
      break;
  }

  if (maxEnt < 0.001)
    item = UINT_MAX;
}

void SuccinctMultibitTreeTRIE::calcDominantItems(set<uint32_t> &items, vector<uint32_t> &ids, vector<uint32_t> &oneCols, vector<uint32_t> &zeroCols, vector<uint32_t> &deleteItems) {
  vector<uint32_t> countOne;
  for (size_t i = 0; i < ids.size(); ++i) {
    vector<uint32_t> &fv = fvs[ids[i]]->second;
    for (size_t j = 0; j < fv.size(); ++j) {
      if (countOne.size() <= fv[j])
	countOne.resize(fv[j] + 1);
      ++countOne[fv[j]];
    }
  }
  for (set<uint32_t>::iterator it = items.begin(); it != items.end(); ++it) {
    uint32_t citem = *it;
    if      (countOne.size() <= citem) {
      zeroCols.push_back(citem);
      deleteItems.push_back(citem);
    }
    else if (countOne[citem] == ids.size()) {
      oneCols.push_back(citem);
      deleteItems.push_back(citem);
    }
    else if (countOne[citem] == 0) {
      zeroCols.push_back(citem);
      deleteItems.push_back(citem);
    }
  }
}

void SuccinctMultibitTreeTRIE::splitData(uint32_t item, vector<uint32_t> &ids, vector<uint32_t> &oneIds, vector<uint32_t> &zeroIds) {
  for (size_t i = 0; i < ids.size(); ++i)  {
    vector<uint32_t> &fv = fvs[ids[i]]->second;
    if (binary_search(fv.begin(), fv.end(), item)) 
      oneIds.push_back(ids[i]);
    else
      zeroIds.push_back(ids[i]);
  }
}

void SuccinctMultibitTreeTRIE::buildMultibitTreeRecursive(uint32_t depth, vector<uint32_t> &ids, set<uint32_t> items, vector<Component*> &components) {
  components.resize(components.size() + 1);
  components[components.size() - 1] = new Component;
  components[components.size() - 1]->depth = depth;
  vector<uint32_t> &cids = components[components.size() - 1]->ids;
  components[components.size() - 1]->leaf = false;

  uint32_t item;
  calcSplitItem(ids, items, item);
  items.erase(item);

  if (item == UINT_MAX) {
    components[components.size() - 1]->leaf = true;
    cids.resize(ids.size());
    for (size_t i = 0; i < ids.size(); ++i)
      cids[i] = fvs[ids[i]]->first;
    return;
  }

  vector<uint32_t> &oneCols  = components[components.size() - 1]->oneCols;
  vector<uint32_t> &zeroCols = components[components.size() - 1]->zeroCols;
  vector<uint32_t> deleteItems;
  calcDominantItems(items, ids, oneCols, zeroCols, deleteItems);
  for (size_t i = 0; i < deleteItems.size(); ++i) 
    items.erase(deleteItems[i]);
  deleteItems.clear();

  if (items.size() == 0 || ids.size() <= minsup) {
    components[components.size() - 1]->leaf = true;
    cids.resize(ids.size());
    for (size_t i = 0; i < ids.size(); ++i)
      cids[i] = fvs[ids[i]]->first;
    return;
  }

  vector<uint32_t> oneIds, zeroIds;
  splitData(item, ids, oneIds, zeroIds);
  buildMultibitTreeRecursive(depth + 1, oneIds, items, components);
  buildMultibitTreeRecursive(depth + 1, zeroIds, items, components);
}

void SuccinctMultibitTreeTRIE::save(ofstream &os) {
  {
    size_t size = trees.size();
    os.write((const char*)(&size), sizeof(size_t));
    for (size_t i = 0; i < size; ++i) 
      trees[i].save(os);
  }
  {
    size_t size = remains.size();
    os.write((const char*)(&size), sizeof(size_t));
    for (size_t i = 0; i < size; ++i) {
      remains[i].save(os);
    }
  }

  vb_loud.Save(os);
  items.save(os);
  idconverter.save(os);

  {
    size_t size = clusters.size();
    os.write((const char*)(&size), sizeof(size));
    for (size_t i = 0; i < size; ++i) {
      vector<uint32_t> &cluster = clusters[i];
      size_t size2 = cluster.size();
      os.write((const char*)(&size2), sizeof(size2));
      os.write((const char*)(&cluster[0]), sizeof(uint32_t)*size2); 
    }
  }
}

void SuccinctMultibitTreeTRIE::load(ifstream &is) {
  {
    size_t size;
    is.read((char*)(&size), sizeof(size_t));
    trees.resize(size);
    for (size_t i = 0; i < size; ++i)
      trees[i].load(is);
  }
  {
    size_t size;
    is.read((char*)(&size), sizeof(size_t));
    remains.resize(size);
    for (size_t i = 0; i < size; ++i)
      remains[i].load(is);
  }

  vb_loud.Load(is);
  items.load(is);
  idconverter.load(is);

  {
    size_t size;
    is.read((char*)(&size), sizeof(size));
    clusters.resize(size);
    for (size_t i = 0; i < size; ++i) {
      vector<uint32_t> &cluster = clusters[i];
      size_t size2;
      is.read((char*)(&size2), sizeof(size2));
      cluster.resize(size2);
      is.read((char*)(&cluster[0]), sizeof(uint32_t)*size2); 
    }
  }
}

void SuccinctMultibitTreeTRIE::unique() {
  if (fvs.size() <= 1)
    return;
  size_t size = fvs.size();
  size_t bef = 0;
  std::vector<std::pair<uint32_t, std::vector<uint32_t> >* > newfvs;
  newfvs.push_back(fvs[0]);
  for (size_t i = 1; i < size; ++i) {
    if (fvs[bef]->second != fvs[i]->second) {
      size_t id = fvs[bef]->first;
      if (clusters.size() <= id)
	clusters.resize(id+1);
      for (size_t j = bef; j < i; ++j) 
	clusters[id].push_back(fvs[j]->first);
      newfvs.push_back(fvs[bef]);
      bef = i;
    }
  }
  size_t id = fvs[bef]->first;
  if (clusters.size() <= id)
    clusters.resize(id+1);
  for (size_t j = bef; j < size; ++j) 
    clusters[id].push_back(fvs[j]->first);
  newfvs.push_back(fvs[bef]);

  fvs = newfvs;
}

void SuccinctMultibitTreeTRIE::buildTrie() {
  sort(fvs.begin(), fvs.end(), Cmp3());
  unique();

  vector<uint32_t> tmpidconverter;
  tmpidconverter.resize(fvs.size());
  uint64_t posVB = 0;
  vb_louddb.PushBack(1); posVB++;
  vb_louddb.PushBack(0); posVB++;

  queue<QueueElem> q;
  q.push(QueueElem(0, fvs.size(), 0));

  vector<uint32_t> titems;
  while (!q.empty()) {
    QueueElem &elem = q.front();
    const uint32_t depth = elem.depth;
    const uint32_t left  = elem.left;
    const uint32_t right = elem.right;
    q.pop();

    if (left+1 == right) {
      if (tmpidconverter.size() <= fvs[left]->first)
	tmpidconverter.resize(fvs[left]->first+1);
      tmpidconverter[fvs[left]->first] = posVB;
      
      if (remains.size() <= fvs[left]->first) 
	remains.resize(fvs[left]->first+1);

      SucArray &remain = remains[fvs[left]->first];
      vector<uint32_t> &fv = fvs[left]->second;

      uint32_t bef = 0;
      if (depth > 0)
	bef = fv[depth-1];
      for (size_t d = depth; d < fv.size(); ++d) {
	remain.push(fv[d]-bef);
	bef = fv[d];
      }

      vb_louddb.PushBack(0); posVB++;
      continue;
    }
    
    size_t newLeft = left;
    if (fvs[left]->second.size() == depth) {
      while (newLeft < right && fvs[newLeft]->second.size() == depth) 
	newLeft++;
      for (size_t j = left; j < newLeft; ++j) {
	if (tmpidconverter.size() <= fvs[j]->first)
	  tmpidconverter.resize(fvs[j]->first + 1);
	tmpidconverter[fvs[j]->first] = posVB;
      }
      if (newLeft == right) {
	vb_louddb.PushBack(0); posVB++;
	continue;
      }
    }
    
    uint32_t prev = newLeft;
    uint32_t prev_c = fvs[prev]->second[depth];
    for (size_t i = newLeft+1; i < right; ++i) {
      if (prev_c != fvs[i]->second[depth]) {
	titems.push_back(prev_c);
	vb_louddb.PushBack(1); posVB++;
	q.push(QueueElem(prev, i, depth+1));
	prev = i;
	prev_c = fvs[prev]->second[depth];
      }
    }
   
    if (prev != right) {
      titems.push_back(prev_c);
      vb_louddb.PushBack(1); posVB++;
      q.push(QueueElem(prev,right,depth+1));
    }
    vb_louddb.PushBack(0); posVB++;
  }

  vb_louddb.Build(vb_loud);

  compressItems(titems);

  for (size_t i = 0; i < tmpidconverter.size(); ++i) 
    idconverter.push(tmpidconverter[i]);
  idconverter.build();

  for (size_t i = 0; i < remains.size(); ++i) 
    remains[i].build();
}

void SuccinctMultibitTreeTRIE::compressItems(vector<uint32_t> &titems) {
  size_t cur = 2;
  stack<pair<uint32_t, uint32_t> > stk;
  stk.push(make_pair(cur, 0));
  
  while (!stk.empty()) {
    size_t cur = stk.top().first;
    size_t pv  = stk.top().second; 
    stk.pop();
    if (leaf(cur)) {
      continue;
    }

    vector<uint32_t> tvals;
    for (size_t tcur = cur; vb_loud.GetBit(tcur) != 0; tcur++) {
      size_t   pos = vb_loud.Rank(tcur+1, 1) - 2;
      uint32_t tval = titems[pos];
      titems[pos] -= pv;
      size_t newcur = getFirstChild(tcur);
      stk.push(make_pair(newcur, tval));
    }
  }
  for (size_t i = 0; i < titems.size(); ++i) {
    items.push(titems[i]);
  }
  items.build();
}

void SuccinctMultibitTreeTRIE::build(const char *fname, size_t _minsup) {
  minsup = _minsup;
  {
    ifstream ifs(fname);
    if (!ifs) {
      cerr << "cannot open: " << fname << endl;
      exit(1);
    }
    readFile(ifs, fvs);
    ifs.close();
  }

  cerr << "building multibit tree" << endl;
  double bstime = clock();
  buildMultibitTree();
  double betime = clock();

  cerr << "building trie"  << endl;
  double tstime = clock();
  buildTrie();
  double tetime = clock();

  double btime = (betime - bstime)/CLOCKS_PER_SEC;
  double ttime = (tetime - tstime)/CLOCKS_PER_SEC;
  cout << "building time of multibittree (sec):" << btime << endl;
  cout << "building time of trie (sec):" << ttime << endl;
  cout << "total building time (sec):" << (btime + ttime) << endl;
  cout << "succinct multibit tree size (byte):" << getByte() << endl;
  cout << "trie size (byte):" << getTrieByte() << endl;
}

void SuccinctMultibitTreeTRIE::calcColumnInfo(Tree &tree, uint64_t cur, vector<uint32_t> &qfv, uint32_t &oneColNum, uint32_t &zeroColNum) {
  vector<uint32_t> oneCol;
  tree.oneCols[cur].decode(oneCol);
  for (size_t i = 0; i < oneCol.size(); ++i) {
    if (binary_search(qfv.begin(), qfv.end(), oneCol[i]) == false)
      ++oneColNum;
  }

  vector<uint32_t> zeroCol;
  tree.zeroCols[cur].decode(zeroCol);
  for (size_t i = 0; i < zeroCol.size(); ++i) {
    if (binary_search(qfv.begin(), qfv.end(), zeroCol[i]) == true)
      ++zeroColNum;
  }
}

bool SuccinctMultibitTreeTRIE::pruning(Tree &tree, vector<uint32_t> &qfv, uint32_t oneColNum, uint32_t zeroColNum) {
  uint64_t cardA  = tree.cardinality;
  uint64_t cardB  = qfv.size();
  uint64_t common = std::min(cardA - oneColNum, cardB - zeroColNum);
  return (float(common)/float(cardA + cardB - common) < similarity);
}

float SuccinctMultibitTreeTRIE::jaccardSim(uint32_t id, const vector<uint32_t> &qfv) {
  uint64_t  pos   = idconverter.get(id);
  vector<uint32_t> oitems;

  SucArray &remain = remains[id];
  for (int i = remain.getSize() - 1; i >= 0; --i) 
    oitems.push_back(remain.get(i));

  pos = getParent(pos);
  uint32_t item = getItem(pos);
  oitems.push_back(item);
  while (pos >= 2) {
    pos  = getParent(pos);
    if (pos < 2)
      break;
    item = getItem(pos);
    oitems.push_back(item);
  }

  uint64_t common = 0;
  uint64_t all    = 0;
  int      qpos   = 0;
  int      opos   = oitems.size() - 1;
  uint32_t val = oitems[opos];
  while (qpos < (int)qfv.size() && opos >= 0) {
    if     (qfv[qpos] < val) {
      all++; qpos++;
    }
    else if(qfv[qpos] > val) {
      all++; opos--;
      if (opos >= 0)
	val += oitems[opos];
    }
    else {
      common++; all++;
      qpos++; opos--;
      if (opos >= 0)
	val += oitems[opos];
    }
  }
  all += (qfv.size() - qpos);
  all += (opos + 1);

  return float(common)/float(all);  
}

inline bool SuccinctMultibitTreeTRIE::leaf(uint64_t pos) {
  return (vb_loud.GetBit(pos+1) == 0 && vb_loud.GetBit(pos) == 0);
}

inline uint64_t SuccinctMultibitTreeTRIE::getParent(uint64_t pos) {
  return vb_loud.Select(vb_loud.Rank(pos,0)-1, 1);
}

inline uint64_t SuccinctMultibitTreeTRIE::getFirstChild(uint64_t pos) {
  return vb_loud.Select(vb_loud.Rank(pos+1,1)-1, 0) + 1;
}

inline uint32_t SuccinctMultibitTreeTRIE::getItem(uint64_t pos) {
  return items.get(vb_loud.Rank(pos+1, 1) - 2);
}

void SuccinctMultibitTreeTRIE::calcSimilarity(Tree &tree, uint64_t cur, vector<uint32_t> &qfv, vector<pair<float, uint32_t> > &res) {
  vector<uint32_t> ids;
  tree.getIds(cur, ids);
  float sim = 0.f;

  for (size_t id = 0; id < ids.size(); ++id)  {
    if (clusters.size() <= ids[id])
      continue;
    
    vector<uint32_t> &cluster = clusters[ids[id]];

    for (size_t j = 0; j < cluster.size(); ++j) {
      if ((sim = jaccardSim(cluster[0], qfv)) >= similarity)  
	res.push_back(make_pair(sim, cluster[j]));
    }
  }
}

void SuccinctMultibitTreeTRIE::searchQueryRecursive(Tree &tree, uint64_t cur, vector<uint32_t> &qfv, uint32_t oneColNum, uint32_t zeroColNum, vector<pair<float,  uint32_t> > &res) {
  if (tree.leaf(cur)) {
    calcSimilarity(tree, cur, qfv, res);
    return;
  }

  calcColumnInfo(tree, cur, qfv, oneColNum, zeroColNum);
  if (pruning(tree, qfv, oneColNum, zeroColNum)) {
    return;
  }

  searchQueryRecursive(tree, tree.getFirstChild(cur), qfv, oneColNum, zeroColNum, res);
  searchQueryRecursive(tree, tree.getFirstChild(cur+1), qfv, oneColNum, zeroColNum, res);
}

void SuccinctMultibitTreeTRIE::searchQuery(vector<uint32_t> &qfv, vector<pair<float,  uint32_t> > &res) {
  uint64_t queryOneNum = qfv.size();
  for (size_t i = 0; i < trees.size(); ++i) {
    uint32_t cardinality = trees[i].cardinality;
    if (similarity * float(cardinality) <= float(queryOneNum) && float(queryOneNum) <= float(cardinality) / similarity)  {
      vector<uint32_t> tmpvec;
      //      trees[i].Print(tmpvec);
      searchQueryRecursive(trees[i], 2, qfv, 0, 0, res);
    }
    if (similarity * float(cardinality) > float(queryOneNum))
      break;
    
  }
}

void SuccinctMultibitTreeTRIE::search(const char *qname, float _sim) {
  similarity = _sim;

  cerr << "loading query file:" << qname << endl;
  vector<pair<uint32_t, vector<uint32_t> >* > qfvs;
  {
    ifstream ifs(qname);
    if (!ifs) {
      cerr << "cannot open: " << qname << endl;
      exit(0);
    }
    readFile(ifs, qfvs);
    ifs.close();
  }

  uint64_t totalNum = 0;
  vector<double> times;
  for (size_t i = 0; i < qfvs.size(); ++i) {
    uint32_t qid          = qfvs[i]->first;
    vector<uint32_t> &qfv = qfvs[i]->second;
    vector<pair<float,  uint32_t> > res;
    double stime = clock();
    searchQuery(qfv, res);
    double etime = clock();
    times.push_back((etime - stime)/CLOCKS_PER_SEC);
    totalNum += res.size();
    cout << "query id:" << qid << " num:" << res.size() << " ";
    for (size_t j = 0; j < res.size(); ++j) {
      cout << res[j].second << ":" << res[j].first << " ";
    }
    cout << endl;
  }
  double mean = 0.0;
  for (size_t i = 0; i < times.size(); ++i)
    mean += times[i];
  mean /= times.size();

  double var = 0.0;
  for (size_t i = 0; i < times.size(); ++i)
    var += (times[i] - mean) * (times[i] - mean);
  var /= (times.size() - 1);
  var = sqrt(var);

  cout << "average answers:" << double(totalNum)/double(qfvs.size()) << endl;
  cout << "average cpu time (sec):" << mean << endl;
  cout << "variance:" << var << endl;
}
}
