/*
 * Build.cpp
 * Copyright (c) 2013 Yasuo Tabei All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE and
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include "./smbt+trie/SuccinctMultibitTreeTRIE.hpp"
#include "./smbt+vla/SuccinctMultibitTreeVLA.hpp"
#include "./mbt/MultibitTree.hpp"

using namespace std;

char    *fname  = NULL;
char    *oname  = NULL;
uint64_t minsup = 10;
uint64_t mode   = 1;

void usage();
void version();
void parse_parameters (int argc, char **argv);

int main(int argc, char **argv) {
  parse_parameters(argc, argv);

  ofstream ofs(oname, ios::out | ios::binary | ios::trunc);
  if (!ofs) {
    cerr << "cannot open: " << oname << endl;
    exit(0);
  }
  ofs.write((const char*)(&mode), sizeof(mode));
  if      (mode == 1) {
    SMBTTRIE::SuccinctMultibitTreeTRIE mbt;
    mbt.build(fname, minsup);
    mbt.save(ofs);
  }
  else if (mode == 2) {
    SMBTVLA::SuccinctMultibitTreeVLA mbt;
    mbt.build(fname, minsup);
    mbt.save(ofs);
  }
  else if (mode == 3) {
    MBT::MultibitTree mbt;
    mbt.build(fname, minsup);
    mbt.save(ofs);
  }
  else {
    std::cerr << "error : mode should be 1 or 2 or 3." << std::endl;
    return 0;
  }
  ofs.close();

  return 0;
}

void version() {
  std::cerr << "Succinct Multibit Tree version 0.0.1" << std::endl
            << "Written by Yasuo Tabei" << std::endl << std::endl;
}

void usage(){
  std::cerr << std::endl
       << "Usage: smbt-build [OPTION]... DATABASEFILE INDEXFILE" << std::endl << std::endl
       << "       where [OPTION]...  is a list of zero or more optional arguments" << std::endl
       << "             DATABASEFILE  is the name of a graph database" << std::endl
       << "             INDEXFILE     is the name of an indexfile" << std::endl << std::endl
       << "Additional arguments (input and output files may be specified):" << std::endl
       << "       -mode [1|2|3]" << std::endl
       << "       1:Succinct Multibit Tree + Succinct TRIE 2:Succinct Multibit Tree + Variable Length Array 3:Multibit Tree" << std::endl
       << "       (mode: " << mode << ")" << std::endl
       << "       -minsup [value]: minimum number of data at each leaf" << std::endl
       << "       (default: " << minsup << ")" << std::endl
       << std::endl;
  exit(0);
}

void parse_parameters (int argc, char **argv){
  if (argc == 1) usage();
  int argno;
  for (argno = 1; argno < argc; argno++){
    if (argv[argno][0] == '-'){
      if      (!strcmp (argv[argno], "-version")){
	version();
      }
      else if (!strcmp (argv[argno], "-mode")) {
	if (argno == argc - 1) std::cerr << "Must specify mode" << std::endl;
	mode = atoi(argv[++argno]);
      }
      else if (!strcmp (argv[argno], "-minsup")) {
	if (argno == argc - 1) std::cerr << "Must specify # of iterations after -iteration" << std::endl;
	minsup = atoi(argv[++argno]);
      }
      else {
	usage();
      }
    } else {
      break;
    }
  }
  if (argno + 1 >= argc)
    usage();

  fname     = argv[argno];
  oname     = argv[argno + 1];
}
