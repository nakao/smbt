/*
 * Search.cpp
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

char  *qfname    = NULL;
char  *infile    = NULL;
float similarity = 0.9;
uint64_t mode    = 1;

void usage();
void version();
void parse_parameters (int argc, char **argv);

int main(int argc, char **argv) {
  parse_parameters(argc, argv);

  ifstream ifs(infile, ios::in | ios::binary);
  if (!ifs) {
    cerr << "cannot open : " << infile << endl;
  }
  cerr << "Load:" << infile << endl;
  ifs.read((char*)(&mode), sizeof(mode));
  if      (mode == 1) {
    SMBTTRIE::SuccinctMultibitTreeTRIE mbt;
    mbt.load(ifs);
    ifs.close();

    mbt.search(qfname, similarity);
  }
  else if (mode == 2) {
    SMBTVLA::SuccinctMultibitTreeVLA mbt;
    mbt.load(ifs);
    ifs.close();

    mbt.search(qfname, similarity);
  }
  else if (mode == 3) {
    MBT::MultibitTree mbt;
    mbt.load(ifs);
    ifs.close();

    mbt.search(qfname, similarity);
  }
  else {
    cerr << "mode should be 1 or 2 or 3." << endl;
    return 0;
  }

  return 0;
}


void version() {
  std::cerr << "Succinct Multibit Tree version 0.0.1" << std::endl
            << "Written by Yasuo Tabei" << std::endl << std::endl;
}

void usage(){
  std::cerr << std::endl
       << "Usage: smbt-search [OPTION]... INDEXFILE DATABASEFILE" << std::endl << std::endl
       << "       where [OPTION]...  is a list of zero or more optional arguments" << std::endl
       << "             INDEXFILE     is the name of an indexfile" << std::endl
       << "             DATABASEFILE  is the name of a graph database" << std::endl << std::endl
       << "Additional arguments (input and output files may be specified):" << std::endl
       << "       -similarity [value]: threshold of Jaccard-Tanimoto similarity" << std::endl
       << "       (default: " << similarity << ")" << std::endl
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
      else if (!strcmp (argv[argno], "-similarity")) {
	if (argno == argc - 1) std::cerr << "Must specify # of iteration after -iteration" << std::endl;
	similarity = atof(argv[++argno]);
	if (similarity < 0.f || 1.f < similarity) {
	  cerr << "The threshold value must be 0 <= similarity <= 1." << endl;
	  exit(0);
	}
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

  infile = argv[argno];
  qfname = argv[argno + 1];

}
