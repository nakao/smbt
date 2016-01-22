# smbt
Succinct Multibit Tree (copy from https://code.google.com/p/smbt/ )

## Introduction

smbt (1,2,3) is a software for similarity searches of fingerprints. It takes a fingerprint database as an input, and builds an index consisting of multibit trees (MT) (2,3) as an output. The index of MTs are used for searching fingerprints similar to a given query. Standard pointer-based implementations of MTs consume a large amount of memory for large-scale fingerprints, which influences applicability of multibit trees. Recently, we presented several memory-efficient representations of MTs by using succinct data structures (1), and demonstrated their applicability to the PubChem database, one of the largest databases of chemical fingerprints. One might worry about a slow down of search time for reducing memory usage of MTs. We also demonstrated that the search time of our succinct representation of MTs was not so different from that of pointer-based multibit trees, which are presented in our paper.

## Quick Start

To compile smbt, please type the following commands:

    tar -xzvf smbt-X.X.X.tar.bz2
    cd smbt-X.X.X
    make

To make an index from your fingerprint database, the smbt-build command can be used as follows:

    ./prog/smbt-build -mode 1 ./dat/fingerprints.dat index

where fingerprints.dat is a fingerprint database, and index is an index.

To search for fingerprints similar to query fingerprints, the smbt-search command can be used as follows:

    ./prog/smbt-search -similarity 0.8 index ./dat/fingerprints.dat

where -similarity indicates a similarity threshold of 0.8 ; index is the index built from the smbt-build command ; fingerprints.dat is query fingerprints.

## Usage

smbt-build

     Usage: smbt-build [OPTION]... DATABASEFILE INDEXFILE
     
           where [OPTION]...  is a list of zero or more optional arguments
                 DATABASEFILE  is the name of a graph database
                 INDEXFILE     is the name of an indexfile
      
           Additional arguments (input and output files may be specified):
                 -mode [1|2|3]
                       1:Succinct Multibit Tree + Succinct TRIE 2:Succinct Multibit Tree + Variable Length Array 3:Multibit Tree
                       (default: 1)
                 -minsup [value]: minimum number of fingerprints at each leaf
                       (default: 10)

smbt-search

    Usage: smbt-search [OPTION]... INDEXFILE DATABASEFILE
           
           where [OPTION]...  is a list of zero or more optional arguments
                 INDEXFILE     is the name of an indexfile
                 DATABASEFILE  is the name of a graph database
       
           Additional arguments (input and output files may be specified):
                 -similarity [value]: threshold of Jaccard-Tanimoto similarity
                 (default: 0.9)

## Reference

1.  Yasuo Tabei: Succinct Multibit Tree: Compact Representation of Multibit Trees by Using Succinct Data Structures in Chemical Fingerprint Searches, Workshop on Algorithms in Bioinformatics (WABI) ALGO, 2012 Link to the PDF
2. Kristensen, T.G., Nielsen, J., Pedersen, C.N.S.: A Tree Based Method for the Rapid Screening of Chemical Fingerprints. In: Salzberg, S.L., Warnow, T. (eds.) WABI2009. LNCS, vol. 5724, pp. 194–205. Springer, Heidelberg (2009)
3. Kristensen, T.G., Nielsen, J., Pedersen, C.N.S.: A tree-based method for the rapid screening of chemical ﬁngerprints. Algorithms for Molecular Biology 5 (2010)
4. sdic http://code.google.com/p/rsdic/
