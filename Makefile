CXXFLAGS += -g -std=c++11 -pedantic -Wall
#LDFLAGS += -I../jsoncpp/include
TARGET = bin/snake

CPP = $(wildcard *.cpp)
C = $(wildcard *.c)
OBJ = $(CPP:.cpp=.o) $(C:.c=.o)

all : $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJ) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $(CPP)

%.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(C)

.PHONY = clean
clean :
	rm -f $(TARGET)
	rm -f *.o
