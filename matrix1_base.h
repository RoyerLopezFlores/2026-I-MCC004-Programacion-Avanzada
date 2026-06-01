#ifndef __MATRIX2_BASE_H__
#define __MATRIX2_BASE_H__

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

#ifndef MATRIX2_DEBUG
#define MATRIX2_DEBUG 1
#endif

#if MATRIX2_DEBUG
#define MATRIX2_TRACE(msg) do { cout << msg << "\n"; } while (false)
#else
#define MATRIX2_TRACE(msg) do { } while (false)
#endif

template <typename T>
void AddOne2(T &n) { ++n; }

template <typename T>
class Matrix2 {
  private:
    T **m_pMat = nullptr;
    size_t m_rows = 0;
    size_t m_cols = 0;

  public:
    Matrix2() { MATRIX2_TRACE("Matrix2 default constructor"); }
    Matrix2(size_t rows, size_t cols);
    Matrix2(const Matrix2 &other);
    Matrix2(Matrix2 &&other) noexcept;

    Matrix2 &operator=(const Matrix2 &other);
    Matrix2 &operator=(Matrix2 &&other) noexcept;

    ~Matrix2() {
        MATRIX2_TRACE("Matrix2 Destructor");
        Destroy();
    }

    void Create();
    void Destroy();

    size_t Rows() const { return m_rows; }
    size_t Cols() const { return m_cols; }

    istream &Read(istream &is);
    ostream &Print(ostream &os) const;

    T *operator[](size_t row);
    const T *operator[](size_t row) const;

    Matrix2 operator+(const Matrix2 &other) const;
    Matrix2 operator-(const Matrix2 &other) const;
    Matrix2 operator*(const Matrix2 &other) const;

    Matrix2 operator+(const T &scalar) const;
    Matrix2 operator-(const T &scalar) const;
    Matrix2 operator*(const T &scalar) const;

    Matrix2 ElementWiseMultiply(const Matrix2 &other) const;
};

template <typename T>
Matrix2<T>::Matrix2(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
    MATRIX2_TRACE("Matrix2 constructor(size_t,size_t)");
    if (m_rows == 0 || m_cols == 0)
        throw invalid_argument("Dimensiones invalidas para Matrix2");
    Create();
}

template <typename T>
Matrix2<T>::Matrix2(const Matrix2<T> &other) : m_rows(other.m_rows), m_cols(other.m_cols) {
    MATRIX2_TRACE("Matrix2 Copy constructor");
    if (other.m_pMat == nullptr || m_rows == 0 || m_cols == 0) {
        m_pMat = nullptr;
        m_rows = m_cols = 0;
        return;
    }

    Create();
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] = other.m_pMat[i][j];
}

template <typename T>
Matrix2<T>::Matrix2(Matrix2<T> &&other) noexcept
    : m_pMat(exchange(other.m_pMat, nullptr)),
      m_rows(exchange(other.m_rows, 0)),
      m_cols(exchange(other.m_cols, 0)) {
    MATRIX2_TRACE("Matrix2 Move constructor");
}

template <typename T>
Matrix2<T> &Matrix2<T>::operator=(const Matrix2<T> &other) {
    MATRIX2_TRACE("Matrix2 Copy assignment");
    if (this == &other)
        return *this;

    Matrix2<T> temp(other);
    swap(m_pMat, temp.m_pMat);
    swap(m_rows, temp.m_rows);
    swap(m_cols, temp.m_cols);
    return *this;
}

template <typename T>
Matrix2<T> &Matrix2<T>::operator=(Matrix2<T> &&other) noexcept {
    MATRIX2_TRACE("Matrix2 Move assignment");
    if (this != &other) {
        Destroy();
        m_pMat = exchange(other.m_pMat, nullptr);
        m_rows = exchange(other.m_rows, 0);
        m_cols = exchange(other.m_cols, 0);
    }
    return *this;
}

template <typename T>
void Matrix2<T>::Create() {
    MATRIX2_TRACE("Matrix2 Create(" << m_rows << "," << m_cols << ")");
    assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for (size_t i = 0; i < m_rows; ++i)
        m_pMat[i] = new T[m_cols];
}

template <typename T>
void Matrix2<T>::Destroy() {
    MATRIX2_TRACE("Matrix2 Destroy(" << m_rows << "," << m_cols << ")");
    if (m_pMat != nullptr) {
        for (size_t i = 0; i < m_rows; ++i)
            delete[] m_pMat[i];
        delete[] m_pMat;
        m_pMat = nullptr;
    }
    m_rows = 0;
    m_cols = 0;
}

template <typename T>
istream &Matrix2<T>::Read(istream &is) {
    MATRIX2_TRACE("Matrix2 Read");
    Destroy();

    size_t rows = 0;
    size_t cols = 0;

    string firstToken;
    is >> firstToken;
    if (!is) {
        m_rows = m_cols = 0;
        return is;
    }

    if (firstToken != "MATRIZ:" && firstToken != "MATRIZ") {
        m_rows = m_cols = 0;
        is.setstate(ios::failbit);
        return is;
    }

    is >> rows >> cols;
    if (!is || rows == 0 || cols == 0) {
        m_rows = m_cols = 0;
        return is;
    }

    m_rows = rows;
    m_cols = cols;
    Create();

    for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j < m_cols; ++j) {
            if (!(is >> m_pMat[i][j])) {
                Destroy();
                return is;
            }
        }
    }

    return is;
}

template <typename T>
ostream &Matrix2<T>::Print(ostream &os) const {
    MATRIX2_TRACE("Matrix2 Print");
    os << "MATRIZ:\n";
    os << m_rows << " " << m_cols << "\n";
    for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j < m_cols; ++j)
            os << m_pMat[i][j] << " ";
        os << "\n";
    }
    return os;
}

template <typename T>
T *Matrix2<T>::operator[](size_t row) {
    if (m_pMat == nullptr || row >= m_rows)
        throw out_of_range("Indice de fila fuera de rango");
    return m_pMat[row];
}

template <typename T>
const T *Matrix2<T>::operator[](size_t row) const {
    if (m_pMat == nullptr || row >= m_rows)
        throw out_of_range("Indice de fila fuera de rango");
    return m_pMat[row];
}

template <typename T>
Matrix2<T> Matrix2<T>::operator+(const Matrix2<T> &other) const {
    MATRIX2_TRACE("Matrix2 +");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede sumar: dimensiones distintas");
    Matrix2<T> result(m_rows, m_cols);
    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return result;

    
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] + other.m_pMat[i][j];

    return result;
}

template <typename T>
Matrix2<T> Matrix2<T>::operator-(const Matrix2<T> &other) const {
    MATRIX2_TRACE("Matrix2 -");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede restar: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix2<T>();

    Matrix2<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] - other.m_pMat[i][j];

    return result;
}

template <typename T>
Matrix2<T> Matrix2<T>::operator*(const Matrix2<T> &other) const {
    MATRIX2_TRACE("Matrix2 *");
    if (m_cols != other.m_rows)
        throw invalid_argument("No se puede multiplicar: columnas(A) != filas(B)");

    if (m_rows == 0 || m_cols == 0 || other.m_cols == 0)
        return Matrix2<T>();

    Matrix2<T> result(m_rows, other.m_cols);
    for (size_t i = 0; i < result.m_rows; ++i) {
        for (size_t j = 0; j < result.m_cols; ++j) {
            result.m_pMat[i][j] = T{};
            for (size_t k = 0; k < m_cols; ++k)
                result.m_pMat[i][j] += m_pMat[i][k] * other.m_pMat[k][j];
        }
    }

    return result;
}

template <typename T>
Matrix2<T> Matrix2<T>::operator+(const T &scalar) const {
    MATRIX2_TRACE("Matrix2 +s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix2<T>();

    Matrix2<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] + scalar;

    return result;
}

template <typename T>
Matrix2<T> Matrix2<T>::operator-(const T &scalar) const {
    MATRIX2_TRACE("Matrix2 -s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix2<T>();

    Matrix2<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] - scalar;

    return result;
}

template <typename T>
Matrix2<T> Matrix2<T>::operator*(const T &scalar) const {
    MATRIX2_TRACE("Matrix2 *s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix2<T>();

    Matrix2<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = scalar * m_pMat[i][j];

    return result;
}

template <typename T>
Matrix2<T> Matrix2<T>::ElementWiseMultiply(const Matrix2<T> &other) const {
    MATRIX2_TRACE("Matrix2 *ee");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede multiplicar elemento a elemento: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix2<T>();

    Matrix2<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] * other.m_pMat[i][j];

    return result;
}

template <typename T>
istream &operator>>(istream &is, Matrix2<T> &matrix) {
    return matrix.Read(is);
}

template <typename T>
ostream &operator<<(ostream &os, const Matrix2<T> &matrix) {
    return matrix.Print(os);
}

template <typename T>
Matrix2<T> operator*(const T &scalar, const Matrix2<T> &matrix) {
    MATRIX2_TRACE("Matrix2 sd*");
    return matrix * scalar;
}

template <typename T>
Matrix2<T> operator+(const T &scalar, const Matrix2<T> &matrix) {
    MATRIX2_TRACE("Matrix2 sd+");
    return matrix + scalar;
}

template <typename T>
Matrix2<T> operator-(const T &scalar, const Matrix2<T> &matrix) {
    MATRIX2_TRACE("Matrix2 sd-");
    if (matrix.Rows() == 0 || matrix.Cols() == 0)
        return Matrix2<T>();

    Matrix2<T> result(matrix.Rows(), matrix.Cols());
    for (size_t i = 0; i < matrix.Rows(); ++i)
        for (size_t j = 0; j < matrix.Cols(); ++j)
            result[i][j] = scalar - matrix[i][j];

    return result;
}

#endif // __MATRIX2_BASE_H__
