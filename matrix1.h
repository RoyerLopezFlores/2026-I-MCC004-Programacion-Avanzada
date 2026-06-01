#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

#ifndef MATRIX1_DEBUG
#define MATRIX1_DEBUG 1
#endif

#if MATRIX1_DEBUG
#define MATRIX1_TRACE(msg) do { cout << msg << "\n"; } while (false)
#else
#define MATRIX1_TRACE(msg) do { } while (false)
#endif

template <typename T>
void AddOne(T &n) { ++n; }

template <typename T>
class Matrix1 {
  private:
    T **m_pMat = nullptr;
    size_t m_rows = 0;
    size_t m_cols = 0;

  public:
        Matrix1() { MATRIX1_TRACE("default constructor"); }
    Matrix1(size_t rows, size_t cols);
    Matrix1(const Matrix1 &other);
    Matrix1(Matrix1 &&other) noexcept;

    Matrix1 &operator=(const Matrix1 &other);
    Matrix1 &operator=(Matrix1 &&other) noexcept;

    ~Matrix1() {
        MATRIX1_TRACE("Destructor");
        Destroy();
    }

    void Create();
    void Destroy();

    size_t Rows() const { return m_rows; }
    size_t Cols() const { return m_cols; }

    istream &Read(istream &is);

    template <typename Func, typename... Args>
    void ApplyFunctionToAll(Func func, Args &&...args);

    ostream &Print(ostream &os) const;

    T *operator[](size_t row);
    const T *operator[](size_t row) const;

    Matrix1 operator+(const Matrix1 &other) const;
    Matrix1 operator-(const Matrix1 &other) const;
    Matrix1 operator*(const Matrix1 &other) const;
    Matrix1 &operator+=(const Matrix1 &other);
    Matrix1 &operator-=(const Matrix1 &other);
    Matrix1 &operator*=(const Matrix1 &other);

    Matrix1 operator+(const T &scalar) const;
    Matrix1 operator-(const T &scalar) const;
    Matrix1 operator*(const T &scalar) const;
    Matrix1 &operator+=(const T &scalar);
    Matrix1 &operator-=(const T &scalar);
    Matrix1 &operator*=(const T &scalar);

    Matrix1 ElementWiseMultiply(const Matrix1 &other) const;
};

template <typename T>
Matrix1<T>::Matrix1(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
    MATRIX1_TRACE("constructor(size_t,size_t)");
    if (m_rows == 0 || m_cols == 0)
        throw invalid_argument("Dimensiones invalidas para Matrix1");
    Create();
}

template <typename T>
Matrix1<T>::Matrix1(const Matrix1<T> &other) : m_rows(other.m_rows), m_cols(other.m_cols) {
    MATRIX1_TRACE("Copy constructor");
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
Matrix1<T>::Matrix1(Matrix1<T> &&other) noexcept
        : m_pMat(exchange(other.m_pMat, nullptr)),
      m_rows(exchange(other.m_rows, 0)),
            m_cols(exchange(other.m_cols, 0)) {
        MATRIX1_TRACE("Move constructor");
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator=(const Matrix1<T> &other) {
    MATRIX1_TRACE("Copy assignment");
    if (this == &other)
        return *this;

    Matrix1<T> temp(other);
    swap(m_pMat, temp.m_pMat);
    swap(m_rows, temp.m_rows);
    swap(m_cols, temp.m_cols);
    return *this;
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator=(Matrix1<T> &&other) noexcept {
    MATRIX1_TRACE("Move assignment");
    if (this != &other) {
        Destroy();
        m_pMat = exchange(other.m_pMat, nullptr);
        m_rows = exchange(other.m_rows, 0);
        m_cols = exchange(other.m_cols, 0);
    }
    return *this;
}

template <typename T>
void Matrix1<T>::Create() {
    MATRIX1_TRACE("Create(" << m_rows << "," << m_cols << ")");
    assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for (size_t i = 0; i < m_rows; ++i)
        m_pMat[i] = new T[m_cols];
}

template <typename T>
void Matrix1<T>::Destroy() {
    MATRIX1_TRACE("Destroy(" << m_rows << "," << m_cols << ")");
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
istream &Matrix1<T>::Read(istream &is) {
    MATRIX1_TRACE("Read");
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
template <typename Func, typename... Args>
void Matrix1<T>::ApplyFunctionToAll(Func func, Args &&...args) {
    MATRIX1_TRACE("ApplyFunctionToAll");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return;

    for (size_t row = 0; row < m_rows; ++row)
        for (size_t col = 0; col < m_cols; ++col)
            func(m_pMat[row][col], forward<Args>(args)...);
}

template <typename T>
ostream &Matrix1<T>::Print(ostream &os) const {
    MATRIX1_TRACE("Print");
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
T *Matrix1<T>::operator[](size_t row) {
    if (m_pMat == nullptr || row >= m_rows)
        throw out_of_range("Indice de fila fuera de rango");
    return m_pMat[row];
}

template <typename T>
const T *Matrix1<T>::operator[](size_t row) const {
    if (m_pMat == nullptr || row >= m_rows)
        throw out_of_range("Indice de fila fuera de rango");
    return m_pMat[row];
}

template <typename T>
Matrix1<T> Matrix1<T>::operator+(const Matrix1<T> &other) const {
    MATRIX1_TRACE("+");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede sumar: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix1<T>();

    Matrix1<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] + other.m_pMat[i][j];

    return result;
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator+=(const Matrix1<T> &other) {
    MATRIX1_TRACE("+=");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede sumar: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return *this;

    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] += other.m_pMat[i][j];

    return *this;
}

template <typename T>
Matrix1<T> Matrix1<T>::operator-(const Matrix1<T> &other) const {
    MATRIX1_TRACE("-");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede restar: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix1<T>();

    Matrix1<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] - other.m_pMat[i][j];

    return result;
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator-=(const Matrix1<T> &other) {
    MATRIX1_TRACE("-=");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede restar: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return *this;

    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] -= other.m_pMat[i][j];

    return *this;
}

template <typename T>
Matrix1<T> Matrix1<T>::operator*(const Matrix1<T> &other) const {
    MATRIX1_TRACE("*");
    if (m_cols != other.m_rows)
        throw invalid_argument("No se puede multiplicar: columnas(A) != filas(B)");

    if (m_rows == 0 || m_cols == 0 || other.m_cols == 0)
        return Matrix1<T>();

    Matrix1<T> result(m_rows, other.m_cols);
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
Matrix1<T> &Matrix1<T>::operator*=(const Matrix1<T> &other) {
    MATRIX1_TRACE("*=");
    if (m_cols != other.m_rows)
        throw invalid_argument("No se puede multiplicar: columnas(A) != filas(B)");

    if (m_rows == 0 || m_cols == 0 || other.m_cols == 0 || m_pMat == nullptr || other.m_pMat == nullptr) {
        Destroy();
        return *this;
    }

    Matrix1<T> result = (*this) * other;
    *this = move(result);
    return *this;
}

template <typename T>
Matrix1<T> Matrix1<T>::operator+(const T &scalar) const {
    MATRIX1_TRACE("+s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix1<T>();

    //Matrix1<T> result(m_rows, m_cols);

    //for (size_t i = 0; i < m_rows; ++i)
    //    for (size_t j = 0; j < m_cols; ++j)
    //        result.m_pMat[i][j] = m_pMat[i][j] + scalar;

    Matrix1<T> result(*this);
    result.ApplyFunctionToAll([scalar](T &elem) { elem += scalar; });
    
    return result;
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator+=(const T &scalar) {
    MATRIX1_TRACE("+=s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return *this;

    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] += scalar;

    return *this;
}

template <typename T>
Matrix1<T> Matrix1<T>::operator-(const T &scalar) const {
    MATRIX1_TRACE("-s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix1<T>();

    Matrix1<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] - scalar;

    return result;
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator-=(const T &scalar) {
    MATRIX1_TRACE("-=s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return *this;

    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] -= scalar;

    return *this;
}

template <typename T>
Matrix1<T> Matrix1<T>::operator*(const T &scalar) const {
    MATRIX1_TRACE("*s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix1<T>();

    Matrix1<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = scalar * m_pMat[i][j];

    return result;
}

template <typename T>
Matrix1<T> &Matrix1<T>::operator*=(const T &scalar) {
    MATRIX1_TRACE("*=s");
    if (m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return *this;

    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] *= scalar;

    return *this;
}

template <typename T>
Matrix1<T> Matrix1<T>::ElementWiseMultiply(const Matrix1<T> &other) const {
    MATRIX1_TRACE("*ee");
    if (m_rows != other.m_rows || m_cols != other.m_cols)
        throw invalid_argument("No se puede multiplicar elemento a elemento: dimensiones distintas");

    if (m_pMat == nullptr || other.m_pMat == nullptr || m_rows == 0 || m_cols == 0)
        return Matrix1<T>();

    Matrix1<T> result(m_rows, m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[i][j] = m_pMat[i][j] * other.m_pMat[i][j];

    return result;
}

template <typename T>
istream &operator>>(istream &is, Matrix1<T> &matrix) {
    return matrix.Read(is);
}

template <typename T>
ostream &operator<<(ostream &os, const Matrix1<T> &matrix) {
    return matrix.Print(os);
}

template <typename T>
Matrix1<T> operator*(const T &scalar, const Matrix1<T> &matrix) {
    MATRIX1_TRACE("sd*");
    return matrix * scalar;
}

template <typename T>
Matrix1<T> operator+(const T &scalar, const Matrix1<T> &matrix) {
    MATRIX1_TRACE("sd+");
    return matrix + scalar;
}

template <typename T>
Matrix1<T> operator-(const T &scalar, const Matrix1<T> &matrix) {
    MATRIX1_TRACE("sd-");
    if (matrix.Rows() == 0 || matrix.Cols() == 0)
        return Matrix1<T>();

    Matrix1<T> result(matrix.Rows(), matrix.Cols());
    for (size_t i = 0; i < matrix.Rows(); ++i)
        for (size_t j = 0; j < matrix.Cols(); ++j)
            result[i][j] = scalar - matrix[i][j];

    return result;
}

#endif // __MATRIX_H__
