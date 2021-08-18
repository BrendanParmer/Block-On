# Block-On
A Blender add-on for voxelization of meshes with cross-secton viewing.
I started this project because I wanted a way to be able to view each level of a blocky 3D mesh, and Blender's Remesh modifier didn't provide a non-hacky way to do that.
Anyways, I've implemented a voxelization algorithm from Zhang, et.al's 2017 paper "Efficient Voxelization Using Project Optimal Scanline".
Essentially, we loop through each triangle in the mesh, and get a rough estimate of what voxels it intersects using only integer arithmetic.
We use pybind11 to import this library into Blender's Python API so we don't have to build Blender to use this, while not compromising on performance. 

Currently under development (including the README)

HOW TO USE:
-include all the stuff in the include folder
-compile the c++ code into a dll (I use Visual Studio for this)
-rename that to make it a pyd
-make sure your python scripts can actually import the pyd
-load up main.py in the Blender scripting editor
