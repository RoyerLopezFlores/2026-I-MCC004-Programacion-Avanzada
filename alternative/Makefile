TARGET = demo_vtk.exe
SRCS = demo_vtk.cpp
CXX = g++

# Flags de optimización, advertencias y uso de lld para evitar desbordamiento de memoria
CXXFLAGS = -std=c++17 -O1 -Wall -fuse-ld=lld -mconsole

# Detección dinámica y nativa: busca tanto 'vtk' como 'vtk-*' dentro de include
VTK_FOLDER := $(notdir $(firstword $(wildcard C:/msys64/ucrt64/include/vtk*)))

# Inclusiones: Agregamos la raíz de include y la subcarpeta específica de VTK encontrada
INCLUDES = -IC:/msys64/ucrt64/include -IC:/msys64/ucrt64/include/$(VTK_FOLDER)

# Librerías en el orden correcto de enlazado requerido por g++
LIBS = -lvtkInteractionStyle \
       -lvtkRenderingOpenGL2 \
       -lvtkRenderingUI \
       -lvtkRenderingCore \
       -lvtkFiltersSources \
       -lvtkCommonExecutionModel \
       -lvtkCommonDataModel \
       -lvtkCommonCore \
       -lvtksys \
       -lglew32 \
       -lopengl32

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean