CXX = g++

CXXFLAGS = -O3 -g -DNDEBUG -W -Wall -Wno-deprecated -I./

LINKFLAGS = -lm -L./

SRCS1 = \
	Build.cpp

SRCS2 = \
	Search.cpp

SRCS3 = \
	Searchd.cpp


OBJS  = ./rsdic/lib/EnumCoder.o \
	./rsdic/lib/RSDic.o \
	./rsdic/lib/RSDicBuilder.o \
	./lib/SucArray.o \
	./lib/BitArray.o \
	./lib/compressedBlock.o \
	./lib/varByte.o \
	./mbt/MultibitTree.o \
	./smbt+vla/SuccinctMultibitTreeVLA.o \
	./smbt+trie/SuccinctMultibitTreeTRIE.o

OBJS1 = $(SRCS1:%.cpp=%.o)
OBJS2 = $(SRCS2:%.cpp=%.o)
OBJS3 = $(SRCS3:%.cpp=%.o)

all: smbt-build smbt-search smbt-searchd

smbt-build: $(OBJS1)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS1) $(LINKFLAGS) -o smbt-build

smbt-search: $(OBJS2)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS2) $(LINKFLAGS) -o smbt-search

smbt-searchd: $(OBJS3)
		$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS3) $(LINKFLAGS) -o smbt-searchd

debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c $<

clean:
	rm -f smbt-build smbt-search smbt-searchd *.o *~
