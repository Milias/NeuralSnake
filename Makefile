CXXFLAGS += -g -std=c++11 -pedantic -Wall
LDFLAGS += -Ijson -Iheader
TARGET = bin/snake

CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

all : $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $(CPP)

.PHONY = clean
clean :
	rm -f $(TARGET)
	rm -f *.o
