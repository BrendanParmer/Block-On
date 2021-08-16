#include <iostream>
#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include "voxelization.hpp"
namespace py = pybind11;

int main()
{
    py::scoped_interpreter guard{};

    py::exec("print('hello world')");
    return 0;
}

void test()
{
    std::cout << "ITS ALIVE! ALIVE!" << std::endl;
}

PYBIND11_MODULE(block_on, m)
{
    m.doc() = "Module that voxelizes stuff";

    m.def("vox_tri", &voxelizeTriangle, "A function to voxelize a triangle", 
            py::arg("x0"), py::arg("y0"), py::arg("z0"),
            py::arg("x1"), py::arg("y1"), py::arg("z1"),
            py::arg("x2"), py::arg("y2"), py::arg("z2"));
    m.def("init", &init, "Set up octree and initialize depth", py::arg("depth") = 6);
    m.def("test", &test);
    m.def("PyInit_block_on", &PyInit_block_on); //necessary?
}