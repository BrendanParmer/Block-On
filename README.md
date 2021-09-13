<h1>Block-On</h1>
<h2>A Blender add-on for voxelization of meshes with cross-secton viewing.</h2>  
<h3>Motivation</h3>
<p>I started this project because I wanted a way to be able to view a blocky 3D mesh by each level within the Blender ecosystem. Unfortunately, Blender's Remesh modifier doesn't provide a non-hacky way to do that.</p>
<p>Anyways, I've implemented a voxelization algorithm from Zhang, et.al's 2017 paper "Efficient Voxelization Using Project Optimal Scanline". Essentially, we loop through each triangle in the mesh, and get a rough estimate of what voxels it intersects using only integer arithmetic. Each voxel goes into an octree so the user can control the level of detail without having to recalculate over the whole mesh. We use pybind11 to import this library into Blender's Python API so we don't have to build Blender to use this, while not compromising on performance.</p>
<p>This project is currently under development. I hope to get a working prototype relatively soon</p>

<h3>How to use:</h3>
Basic steps (better explanation coming soon as I clean things up):
<ul>
  <li>include all the stuff in the include folder</li>
  <li>compile the C++ code into a dll (I use Visual Studio for this)</li>
  <li>rename that to make it a pyd</li>
  <li>make sure your python scripts can actually import the pyd</li>
  <li>load up main.py in the Blender scripting editor and run it</li>
</ul>
Now if you select an object and click on the Object menu, you'll see an option for Block On
