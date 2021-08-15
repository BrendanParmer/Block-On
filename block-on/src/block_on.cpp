#include <iostream>
#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include "voxelization.hpp"

int main()
{
    pybind11::scoped_interpreter guard{};

    pybind11::exec("print('hello world')");
    return 0;
}

void test()
{
    std::cout << "ITS ALIVE! ALIVE!" << std::endl;
}

//void PyInit_block_on()
//{
//    std::cout << "Block On initialized" << std::endl;
//}

PYBIND11_MODULE(block_on, m)
{
    m.doc() = "Module that voxelizes stuff";

    m.def("vox_tri", &voxelizeTriangle);
    m.def("init", &init);
    m.def("test", &test);
    m.def("PyInit_block_on", &PyInit_block_on);
}