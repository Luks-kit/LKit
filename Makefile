CXX := g++
CXXFLAGS := -Wall -Wextra -g
INCFLAGS := -Iinc

# Build layout
TARGET := build/toy
OBJDIR := build/objects

# Discover all .cpp sources recursively under src/
SRCS := $(shell find src -name "*.cpp")

# Map sources to object files under $(OBJDIR) keeping directory structure
OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean run test dirs
all: dirs $(TARGET)

# Ensure object directories exist
dirs:
	@mkdir -p $(OBJDIR)

# Link
$(TARGET): $(OBJS)
	@mkdir -p $(dir $(TARGET))
	$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

# Generic rule: compile src/.../file.cpp -> build/objects/.../file.o
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCFLAGS)

clean:
	rm -rf build
	find src -name "*.o" -delete || true

run: all
	$(TARGET)

test: all
	$(TARGET) < build/test.toy
	rm -f $(TARGET)
	rm -rf build/objects

run: all
	$(TARGET)

test: build/test.toy $(TARGET)
	$(TARGET) < $< 
