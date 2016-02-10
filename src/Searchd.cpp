#include <iostream>
#include "./smbt+trie/SuccinctMultibitTreeTRIE.hpp"
#include "./smbt+vla/SuccinctMultibitTreeVLA.hpp"
#include "./mbt/MultibitTree.hpp"

using namespace std;

char  *qfname    = NULL;
char  *infile    = NULL;
float similarity = 0.9;
uint64_t mode    = 1;
char  input[256];

void usage();
void version();
void parse_parameters (int argc, char **argv);
void parse_search_parameters(char *input);

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
    while(true) {
      parse_search_parameters(input);
      mbt.search(qfname, similarity);
      cout << "///" << endl;
    }
  }
  else if (mode == 2) {
    SMBTVLA::SuccinctMultibitTreeVLA mbt;
    mbt.load(ifs);
    ifs.close();
    while(true) {
      parse_search_parameters(input);
      mbt.search(qfname, similarity);
      cout << "///" << endl;
    }
  }
  else if (mode == 3) {
    MBT::MultibitTree mbt;
    mbt.load(ifs);
    ifs.close();
    while(true) {
      parse_search_parameters(input);
      mbt.search(qfname, similarity);
      cout << "///" << endl;
    }
  }
  else {
    cerr << "mode should be 1 or 2 or 3." << endl;
    return 0;
  }

  return 0;
}


// "SEARCH 0.7 query.dat"
void parse_search_parameters (char *input){
  reset:
  cin >> input;
  if (strcmp(input, "SEARCH") == 0) {
    goto reset;
  }
  similarity = atof(input);
  if (similarity < 0.f || 1.f < similarity) {
    cerr << "The threshold value must be 0 <= similarity <= 1." << endl;
  }
  if (similarity == 0) {
    goto reset;
  }

  cin >> input;
  qfname = (char *)input;
  cout << "similarity: " << similarity << endl;
  cout << "qfname:     " << qfname << endl;
  cout << endl;
}


void version() {
  std::cerr << "Succinct Multibit Tree version 0.0.1" << std::endl
            << "Written by Yasuo Tabei" << std::endl << std::endl;
}

void usage(){
  std::cerr << std::endl
       << "Usage: smbt-searchd [OPTION]... INDEXFILE" << std::endl << std::endl
       << "       where [OPTION]...  is a list of zero or more optional arguments" << std::endl
       << "             INDEXFILE     is the name of an indexfile" << std::endl
       << "  Options:" << std::endl
       << "     -version" << std::endl
       << "  Search queries are acceptable in stdin following format:" << std::endl
       << "    SEAECH 0.9 graph-database-file.dat" << std::endl
       << std::endl;
  exit(0);
}

void parse_parameters (int argc, char **argv){
  int argno;
  for (argno = 1; argno < argc; argno++){
    if (argv[argno][0] == '-'){
      if      (!strcmp (argv[argno], "-version")){
        version();
      }
    } else {
      break;
    }
  }
//  if (argno + 1 >= argc)
  //  usage();

  infile = argv[argno];
}
