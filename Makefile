CXX=g++
CXXFLAGS=-Wall -Wextra -g 
INCFLAGS=-Iinc
TARGET=build/toy
TESTERFILE:= $(build/test.toy)
TESTOUTPUT:=$(build/testresult.toy)
SRC:= $(wildcard src/*.cpp)
OBJ:= $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(CXXFLAGS) -o $(TARGET)

src/%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCFLAGS)

clean:
	rm -f $(TARGET)
	rm -f src/*.o
run: all
	$(TARGET)
test: build/test.toy $(TARGET)
	$(TARGET) < $< 
