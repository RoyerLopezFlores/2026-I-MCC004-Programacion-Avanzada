CXX = g++
CXXFLAGS = -std=c++2b -Wall -g -pthread  # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
SRCS = main.cpp util.cpp \
       complex.cpp \
       shapes/shape.cpp \
       shapes/rectangle.cpp \
       shapes/circle.cpp \
       shapes/triangle.cpp \
       shapes/square.cpp \
       polimorfismo.cpp \
       BitSigno.cpp \
       Pointers.cpp \
       Matriz.cpp \
       array1.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean