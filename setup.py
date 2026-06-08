from pybind11.setup_helpers import Pybind11Extension
from setuptools import setup

ext_modules = [
    Pybind11Extension(
        "matrix1",
        ["py_matrix.cpp"],
        cxx_std=20,
        define_macros=[
            ("MATRIX1_DEBUG", "0")
        ]
    ),
]

setup(
    name="matrix1",
    ext_modules=ext_modules,
)