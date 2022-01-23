# Block-On

## A Blender add-on for procedural voxelization of meshes with cross-secton viewing. 

VIDEO TUTORIAL COMING SOON

### Prerequisites
* Just Blender 3.0!

### Features

* Automatically scales the modified mesh to align with Blender's coordinate system for easy visibility
* Control over the size of each instanced cube
* Unlike the Remesh modifier that comes with Blender, the Block-On add-on preserves the original Materials of the mesh, no need to reapply them later!
* View height cross-sections, helpful for procedural animations and building the mesh block by block as a blueprint in voxel-based games like Minecraft

### How to use

* Make sure you're using Blender 3.0, as older versions do not have the necessary geometry nodes
* Download the appropriate file for your Blender installation ( `block_on.py` for Blender 3.0, `block_on_3-1.py` for the Blender 3.1 alpha)
* To enable the add-on, open Blender and go to Edit>Preferences>Add-ons, and click the Install button in the top-right. From there, navigate to where you downloaded the python file, and click on it to install
* Now that you have the Block-On add-on enabled, you can use it by selecting any mesh, going to Object, and hitting the Block-On button. This automatically generates a Geometry Nodes group for your object, with parameters you can adjust in the Modifiers tab

### Parameters
* **Resolution**: the maximum number of blocks along a dimension
  * Default: 32
  * Minimum: 1
  * Maximum: 256
* **Cube Size**: The dimensions of the instanced cube
  * Default: 0.9
  * Minimum: 0.0
  * Maximum: 1.0
* **Level**: The lowest level visible (indexed at 0)
  * Default: 0
  * Minimum: 0
  * Maximum: 256
* **Levels Visible**: The number of layers in each cross-section
  * Default: 32
  * Minimum: 0
  * Maximum: 256
* **Density**: Parameter that specifices how many samples to take from the mesh faces. Higher numbers will be more accurate, but less perfomant.
  * Default: 10.0
  * Minimum: 0.0
  * Maximum: 100.0
* **Seed**: Parameter that changes the randomization used to take the point samples. Helpful for generating slightly different variations of the blocky mesh
  * Default: 0
* **On Points**: Parameter determining whether cubes should be instanced to fall within Blender's coordinate boxes (0), or instances on the grid points (1)
  * Default: 0 (in boxes)

### FAQs
Why aren't the cubes in the right location/orientation/size?
* This is typically due to not applying transforms to the mesh. You can check in the Object Properties tab to see the transforms are default, and if not, press CTRL-A to apply All Transforms to the mesh. The add-on will update the mesh automatically

How do I change the parameter limits?
* This is generally not recommended due to excessively large values causing Blender to crash. However, if need be you can open the Block On node group in a Geometry Nodes tab and navigate to the Group panel. From there, you can see the list of inputs and outputs. Click on the input you want to modify, and you can specify your needed constraints for your project

I added another material to my mesh, but it isn't updating. Why?
* To keep a procedural workflow within the existing Geometry Nodes framework, the Block-On add-on needs to separate the faces of the mesh by all its materials when the node group is initialized. To do this, it loops through the existing materials in the mesh and creates a sub-node group for each one
* To update the mesh, simply reapply the Block-On function in the Object tag. The add-on will automatically remove the previous node groups from your blend file and replace them with new ones

Why is the node group not generating?
* Your version of Blender may be outdated for the release of Block-On you downloaded. Blender's development of Geometry Nodes has been exciting and fast-paced, with many nodes being added and deleted as the feature develops. Support is officially available for Blender 3.0, and there is an experiemental 3.1 version if you are using the alpha. 

What are you planning on adding for the future?
* Currently, the add-on works best for materials that use Generate Textured Coordinates. I'd like to be able to expand this to all materials in the near future, especially with Image Textures.
* I'd also like to add an option that allows a user to reduced a material down to a single color per cube to further achieve the stylized, blocky look. This is possible by using a Snap Vector Math node in a new material, and may be implemented soon.
