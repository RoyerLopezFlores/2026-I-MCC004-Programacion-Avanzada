#include <iostream>
#include <cuda_runtime.h>
#include <cstdlib>
#include <ctime>


#define DIMF_A 1024  // Filas de A (y filas de C)
#define DIMC_A 1024  // Columnas de A (y filas de B)


typedef float DataType; 

#define BLOCK_SIZE 16
#define NUM_REPETICIONES 10


__global__ void matrixMulKernel(const DataType* A, const DataType* B, DataType* C, int numF_A, int numC_A, int numC_B) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < numF_A && col < numC_B) {
        DataType value = 0;
        for (int k = 0; k < numC_A; ++k) {
            value += A[row * numC_A + k] * B[k * numC_B + col];
        }
        C[row * numC_B + col] = value;
    }
}


void inicializarMatriz(DataType* mat, int filas, int columnas) {
    for (int i = 0; i < filas * columnas; i++) {
        if (std::is_same<DataType, float>::value || std::is_same<DataType, double>::value) {
            mat[i] = static_cast<DataType>(rand()) / RAND_MAX;
        } else {
            mat[i] = static_cast<DataType>(rand() % 10);
        }
    }
}

int main() {
    srand(time(NULL));

    int filas_A = DIMF_A;
    int cols_A = DIMC_A;
    int filas_B = DIMC_A; // Requisito algebraico
    int cols_B = DIMF_A; // Definido para este ejemplo
    int filas_C = filas_A;
    int cols_C = cols_B;

    size_t size_A = filas_A * cols_A * sizeof(DataType);
    size_t size_B = filas_B * cols_B * sizeof(DataType);
    size_t size_C = filas_C * cols_C * sizeof(DataType);

    // 1. Asignar memoria en el Host (CPU)
    DataType *h_A = (DataType*)malloc(size_A);
    DataType *h_B = (DataType*)malloc(size_B);
    DataType *h_C = (DataType*)malloc(size_C);

    // 2. Inicializar matrices con datos aleatorios
    inicializarMatriz(h_A, filas_A, cols_A);
    inicializarMatriz(h_B, filas_B, cols_B);

    // 3. Asignar memoria en el Device (GPU)
    DataType *d_A, *d_B, *d_C;
    cudaMalloc((void**)&d_A, size_A);
    cudaMalloc((void**)&d_B, size_B);
    cudaMalloc((void**)&d_C, size_C);

    // 4. Copiar datos de CPU a GPU
    cudaMemcpy(d_A, h_A, size_A, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size_B, cudaMemcpyHostToDevice);

    // 5. Configurar la ejecución (Hilos y Bloques)
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((cols_C + BLOCK_SIZE - 1) / BLOCK_SIZE, (filas_C + BLOCK_SIZE - 1) / BLOCK_SIZE);

    // 6. Configurar Eventos de CUDA para la medición del tiempo
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    std::cout << "Calculando promedio de " << NUM_REPETICIONES << " ejecuciones en GPU..." << std::endl;

   
    cudaEventRecord(start);

    for (int i = 0; i < NUM_REPETICIONES; i++) {
        matrixMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, filas_A, cols_A, cols_B);
    }

    cudaEventRecord(stop);


    // Sincronizar para asegurar que la GPU terminó todo antes de calcular el tiempo
    cudaEventSynchronize(stop);

    float msecTotal = 0.0f;
    cudaEventElapsedTime(&msecTotal, start, stop);

    // Calcular el promedio
    float msecPromedio = msecTotal / NUM_REPETICIONES;

    std::cout << "\n================ RESULTADOS ================" << std::endl;
    std::cout << "Dimensiones de A: " << filas_A << " x " << cols_A << std::endl;
    std::cout << "Dimensiones de B: " << filas_B << " x " << cols_B << std::endl;
    std::cout << "Tiempo Total (" << NUM_REPETICIONES << " iteraciones): " << msecTotal << " ms" << std::endl;
    std::cout << "Tiempo Promedio por multiplicación: " << msecPromedio << " ms" << std::endl;
    std::cout << "============================================" << std::endl;

    // 7. Copiar resultado final de vuelta a la CPU (Opcional, si quieres verificar)
    cudaMemcpy(h_C, d_C, size_C, cudaMemcpyDeviceToHost);

    // Limpieza de memoria
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    free(h_A);
    free(h_B);
    free(h_C);

    return 0;
}