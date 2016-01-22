CXX = g++

CXXFLAGS = -O3 -DNDEBUG -Wno-deprecated -I./ -L./
LINKFLAGS = -lm

SRCS = \
	SucArray.cpp \
	BitArray.cpp \
	compressedBlock.cpp \
	varByte.cpp \

OBJS = $(SRCS:%.cpp=%.o)

all: $(OBJS)

clean:
	rm -f *.o *~
