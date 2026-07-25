CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic
TARGET := demonew
SRC := demonew.cpp

.PHONY: all build run clean rebuild

all: build

build:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe

rebuild: clean build
