#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
//#include <pybind11/stl.h>
#include <sstream>
#include "Matrix1.h"
#include <pybind11/numpy.h>

namespace py = pybind11;
using TD = double;
using Matrix = Matrix1<TD>;
using py_ssize_t = py::ssize_t;
using TIDX = size_t;
PYBIND11_MODULE(matrix1, m) {
    py::class_<Matrix>(m, "Matrix1", py::buffer_protocol())
        .def(py::init<size_t, size_t>())
        .def(py::init<size_t, size_t, const TD &>())
        .def("rows", &Matrix::Rows)
        .def("cols", &Matrix::Cols)
        .def_static("zeros", &Matrix::Zeros)
        .def_static("ones", &Matrix::Ones)
        //m[i]
        .def("__getitem__",
        [](Matrix& mat, size_t row)
        {
            if (row >= mat.Rows())
                throw py::index_error();

            return py::memoryview::from_buffer(
                mat[row],                                        // double*
                { static_cast<py_ssize_t>(mat.Cols()) },        // shape
                { static_cast<py_ssize_t>(sizeof(TD)) }     // stride
            );
        })
        // m[i,j]
        .def("__getitem__",
        [](Matrix& mat, std::pair<TIDX,TIDX> idx)
        {
            auto [r,c] = idx;

            if (r >= mat.Rows() || c >= mat.Cols())
                throw py::index_error();

            return mat[r][c];
        })
        //m[i,j]
        .def("__setitem__",
        [](Matrix& mat, std::pair<TIDX,TIDX> idx, TD value)
        {
            auto [r,c] = idx;

            if (r >= mat.Rows() || c >= mat.Cols())
                throw py::index_error();

            mat[r][c] = value;
        })
        // np.asarray(mat)
        .def_buffer(
        [](Matrix& mat)
        {
            return py::buffer_info(
                mat.Data(),
                sizeof(TD),
                py::format_descriptor<TD>::format(),
                2,
                {
                    static_cast<py_ssize_t>(mat.Rows()),
                    static_cast<py_ssize_t>(mat.Cols())
                },
                {
                    static_cast<py_ssize_t>(
                        sizeof(TD) * mat.Cols()),
                    static_cast<py_ssize_t>(
                        sizeof(TD))
                }
            );
        })



        
        .def("__getitem__", []( Matrix& mat,
                               std::pair<TIDX,TIDX> idx) {
            return mat[idx.first][idx.second];
        })
        
        .def("__setitem__", [](Matrix& mat,
                               std::pair<TIDX,TIDX> idx,
                               double value) {
            mat[idx.first][idx.second] = value;
        })
        .def("__eq__", &Matrix::operator==)
        .def("__ne__", &Matrix::operator!=)
        .def("__str__", [](const Matrix& mat) {
            std::ostringstream oss;
            oss << mat;
            return oss.str();
        })
        .def("__repr__", [](const Matrix& mat) {
            std::ostringstream oss;
            oss << mat;
            return oss.str();
        })

        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * py::self)

        .def(py::self + TD())
        .def(py::self - TD())
        .def(py::self * TD())

        .def(TD() + py::self)
        .def(TD() - py::self)
        .def(TD() * py::self)

        

        ;

    // Convenience aliases to allow matrix1.zeros(...) and matrix1.ones(...)
    m.def("zeros", &Matrix::Zeros);
    m.def("ones", &Matrix::Ones);
}