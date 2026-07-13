#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <thread>
#include <vector>

#include "types.h"
#include "matrix1.h"
#include "matrix1_base.h"

using namespace std;

constexpr int kBenchmarkIterations = 10;
constexpr unsigned int kNativeParallelThreads = 8;

template <typename Func>
double MeasureAverageTimeMicroseconds(Func operation, int iterations = kBenchmarkIterations) {
    if (iterations <= 0) {
        return 0.0;
    }

    using Clock = chrono::high_resolution_clock;
    chrono::duration<double, micro> totalTime(0.0);

    for (int i = 0; i < iterations; ++i) {
        const auto start = Clock::now();
        operation();
        const auto end = Clock::now();
        totalTime += (end - start);
    }

    return totalTime.count() / iterations;
}

template <typename Iterator, typename Func>
void ParallelForEachNative(Iterator first, Iterator last, Func func, unsigned int threadCount = kNativeParallelThreads) {
    const auto totalElements = distance(first, last);
    if (totalElements <= 0) {
        return;
    }

    if (threadCount == 0) {
        threadCount = 1;
    }

    if (static_cast<decltype(totalElements)>(threadCount) > totalElements) {
        threadCount = static_cast<unsigned int>(totalElements);
    }

    const auto baseBlockSize = totalElements / threadCount;
    const auto remainder = totalElements % threadCount;

    vector<thread> workers;
    workers.reserve(threadCount);

    Iterator blockBegin = first;
    for (unsigned int t = 0; t < threadCount; ++t) {
        const auto currentBlockSize = baseBlockSize + (t < static_cast<unsigned int>(remainder) ? 1 : 0);
        Iterator blockEnd = blockBegin;
        advance(blockEnd, currentBlockSize);

        workers.emplace_back([blockBegin, blockEnd, &func]() {
            for_each(blockBegin, blockEnd, func);
        });

        blockBegin = blockEnd;
    }

    for (auto &worker : workers) {
        worker.join();
    }
}

void DemoApplyFunctionBenchmarks() {
    cout << "\nBenchmark ApplyFunctionToAll vs for_each\n";

    constexpr size_t rows = 2048;
    constexpr size_t cols = 2048;
    Matrix1<TI> base(rows, cols, 1);

    const int iterations = kBenchmarkIterations;

    const double avgApplyUs = MeasureAverageTimeMicroseconds([&]() {
        Matrix1<TI> m(base);
        m.ApplyFunctionToAll([](TI &elem) { elem += 2; });
    }, iterations);

    const double avgForEachUs = MeasureAverageTimeMicroseconds([&]() {
        Matrix1<TI> m(base);
        for_each(m.begin(), m.end(), [](TI &elem) { elem += 2; });
    }, iterations);

    cout << "ApplyFunctionToAll promedio (" << iterations << " iteraciones): "
         << avgApplyUs << " us\n";
    cout << "for_each secuencial promedio (" << iterations << " iteraciones): "
         << avgForEachUs << " us\n";

    const double avgForEachParallelUs = MeasureAverageTimeMicroseconds([&]() {
        Matrix1<TI> m(base);
        ParallelForEachNative(m.begin(), m.end(), [](TI &elem) { elem += 2; }, kNativeParallelThreads);
    }, iterations);

    cout << "for_each paralelo (" << kNativeParallelThreads << " hilos) promedio ("
         << iterations << " iteraciones): "
         << avgForEachParallelUs << " us\n";
}

void DemoMatrixType1And2() {
    cout << "Demo Matrix1 vs Matrix2\n";

    istringstream aData(
        "MATRIZ:\n"
        "2 2\n"
        "1 2\n"
        "3 4\n"
    );
    istringstream bData(
        "MATRIZ:\n"
        "2 2\n"
        "5 6\n"
        "7 8\n"
    );
    istringstream dData(
        "MATRIZ:\n"
        "2 2\n"
        "1 1\n"
        "1 1"
    );

    Matrix1<TI> a1, b1, d1;
    Matrix2<TI> a2, b2, d2;

    if (!(aData >> a1) || !(bData >> b1) || !(dData >> d1)) {
        cout << "Error leyendo matrices para Matrix1\n";
        return;
    }

    istringstream aData2(
        "MATRIZ:\n"
        "2 2\n"
        "1 2\n"
        "3 4\n"
    );
    istringstream bData2(
        "MATRIZ:\n"
        "2 2\n"
        "5 6\n"
        "7 8\n"
    );
    istringstream dData2(
        "MATRIZ:\n"
        "2 2\n"
        "1 1\n"
        "1 1"
    );

    if (!(aData2 >> a2) || !(bData2 >> b2) || !(dData2 >> d2)) {
        cout << "Error leyendo matrices para Matrix2\n";
        return;
    }
    cout<<"Matrices leidas correctamente\n";
    cout<<"A1:\n"<<a1<<endl;
    cout<<"B1:\n"<<b1<<endl;
    cout<<"D1:\n"<<d1<<endl;

    try {
        cout << "\n[Matrix1]\n";
        Matrix1<TI> sum1 = a1 + b1;
        Matrix1<TI> sub1 = a1 - b1;
        Matrix1<TI> mul1 = a1 * b1;
        cout<<"Operaciones lineales (A+A)*B*2 + 4 - D:\n\n";
        Matrix1<TI> lin1 = (a1 + a1) * b1 * 2 + 4 - d1;
        cout<<"Fin de la combinacion lineal\n\n";
        cout << "A + B:\n" << sum1;
        cout << "A - B:\n" << sub1;
        cout << "A * B:\n" << mul1;
        cout << "(A+A)*B*2 + 4 - D:\n" << lin1;

        cout << "\n[Matrix2]\n";
        Matrix2<TI> sum2 = a2 + b2;
        Matrix2<TI> sub2 = a2 - b2;
        Matrix2<TI> mul2;
        const int iterations = kBenchmarkIterations;
        const double avgMul2TimeUs = MeasureAverageTimeMicroseconds([&]() {
            mul2 = a2 * b2;
        }, iterations);
        cout << "Promedio de tiempo A2 * B2 (" << iterations << " iteraciones): "
             << avgMul2TimeUs << " us\n";
        Matrix2<TI> lin2 = (a2 + a2) * b2 * 2 + 4 - d2;

        cout << "A + B:\n" << sum2;
        cout << "A - B:\n" << sub2;
        cout << "A * B:\n" << mul2;
        cout << "(A+A)*B*2 + 4 - D:\n" << lin2;
        cout<<"Fin de la combinacion lineal\n";
        cout<<a1<<endl;

        cout<<"ApplyFunctionToAll + 2"<<endl;
        a1.ApplyFunctionToAll([](TI &elem) { elem += 2; });
        cout << "A1 + 2:\n" << a1;
        cout<<"A1 * B1:\n" << a1 * b1;
        cout<<"A1 *A1:\n";
        a1 *= a1;
        cout << a1;
        cout<<endl;
        cout<<"Acceso a elemento A1[0][1]: "<<a1[0][1]<<endl;
        cout<<"Acceso a elemento A1[0]"<< a1[0] <<endl;

    } catch (const exception &ex) {
        cout << "Error en demo Matrix1/Matrix2: " << ex.what() << "\n";
    }

    DemoApplyFunctionBenchmarks();
    /*
    Benchmark ApplyFunctionToAll vs for_each
    ApplyFunctionToAll promedio (10 iteraciones): 90976.8 us
    for_each secuencial promedio (10 iteraciones): 43879.7 us
    for_each paralelo (8 hilos) promedio (10 iteraciones): 26745.9 us
    
    after mutex
    ApplyFunctionToAll promedio (10 iteraciones): 76803.7 us
for_each secuencial promedio (10 iteraciones): 58026.6 us
for_each paralelo (8 hilos) promedio (10 iteraciones): 37468.8 us
    
    */
}
