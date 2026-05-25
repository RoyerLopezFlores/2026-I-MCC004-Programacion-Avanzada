#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <cstddef>
#include <cassert>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

template <typename T>
void AddOne(T &n) { ++n; }

template <typename T>
class Matrix1 {
    private:
        T      **m_pMat = nullptr;
        size_t   m_rows = 0, m_cols = 0;
    public:
        Matrix1()      { }
        ~Matrix1()     { Destroy(); }
        void     Create();
        istream &Read(istream &is);
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args);
        ostream &Print(ostream &os);
        void Destroy();
};

template <typename T>
void Matrix1<T>::Create()
{   assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for(size_t i = 0 ; i < m_rows ; ++i)
        m_pMat[i] = new T[m_cols];
}

template <typename T>
istream &Matrix1<T>::Read(istream &is) {
    Destroy();
    is >> m_rows >> m_cols;
    if (!is || m_rows == 0 || m_cols == 0) {
        m_rows = m_cols = 0;
        return is;
    }

    Create();
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            is >> m_pMat[i][j];

    return is;
}

template <typename T>
template <typename Func, typename... Args>
void Matrix1<T>::ApplyFunctionToAll(Func func, Args&& ...args) {
    //if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
    //    return;
//
    //const size_t workersCount = min(static_cast<size_t>(thread::hardware_concurrency() == 0 ? 2 : thread::hardware_concurrency()), m_rows);
//
    //mutex rowMutex;
    //size_t nextRow = 0;
    //vector<thread> workers;
    //workers.reserve(workersCount);
//
    //for (size_t t = 0; t < workersCount; ++t) {
    //    workers.emplace_back([&]() {
    //        while (true) {
    //            size_t row = 0;
    //            {
    //                scoped_lock lock(rowMutex);
    //                if (nextRow >= m_rows)
    //                    return;
    //                row = nextRow++;
    //            }
//
    //            for (size_t col = 0; col < m_cols; ++col)
    //                func(m_pMat[row][col], args...);
    //        }
    //    });
    //}
//
    //for (thread &worker : workers)
    //    worker.join();
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return;

    for (size_t row = 0; row < m_rows; ++row)
        for (size_t col = 0; col < m_cols; ++col)
            func(m_pMat[row][col], args...);
}

template <typename T>
ostream &Matrix1<T>::Print(ostream &os) {
    os << m_rows << " " << m_cols << "\n";
    for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j < m_cols; ++j)
            os << m_pMat[i][j] << " ";
        os << "\n";
    }
    return os;
}

template <typename T>
void Matrix1<T>::Destroy() {
    if (m_pMat != nullptr) {
        for (size_t i = 0; i < m_rows; ++i)
            delete[] m_pMat[i];
        delete[] m_pMat;
        m_pMat = nullptr;
    }
    m_rows = m_cols = 0;
}

template <typename T>
istream &operator>>(istream &is, Matrix1<T> &matrix) {
    return matrix.Read(is);
}

template <typename T>
ostream &operator<<(ostream &os, Matrix1<T> &matrix) {
    return matrix.Print(os);
}



#endif // __MATRIX_H__