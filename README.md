# Block-On

 A Blender add-on for procedural voxelization of meshes with cross-secton viewing. 

## A Blender add-on for procedural voxelization of meshes with cross-secton viewing. 
<img 
     src="https://github.com/BrendanParmer/Block-On/blob/master/imgs/build_animation.gif" 
     alt="Build animation" 
     width = "850" 
     height = "850"
    >
<table style="margin-left: auto; margin-right: auto;">
  <tr>
    <td>
      <img 
        src="https://github.com/BrendanParmer/Block-On/blob/master/imgs/suzanne_before.png" 
        alt ="Suzanne Before" 
        width="400" 
        height = "400">
    </td>
    <td>
      <img 
        src="https://github.com/BrendanParmer/Block-On/blob/master/imgs/suzanne_after.png" 
        alt = "Suzanne After" 
        width = "400" 
        height="400">
    </td>
  </tr>
  <tr>
    <td>
      <p style="text-align: center;">Before</p>
    </td>
    <td>
      <p style="text-align: center;">After</p>
    </td>
  </tr>
</table>
<table style="margin-left: auto; margin-right: auto;">
  <tr>
    <td>
      <img 
        src="https://github.com/BrendanParmer/Block-On/blob/master/imgs/lizard_before.png" 
        alt="Lizard Before" 
        width="400" 
        height="400">
    </td>
    <td>
      <img 
        src="https://github.com/BrendanParmer/Block-On/blob/master/imgs/lizard_after.png" 
        alt="Lizard After" 
        width="400" 
        height="400">
    </td>
  </tr>
  <tr>
    <td>
      <p style="text-align: center;">Before</p>
    </td>
    <td>
      <p style="text-align: center;">After</p>
    </td>
  </tr>
 </table>
 <p>"Lizard Mage" (https://skfb.ly/68X67) by Enalrem is licensed under Creative Commons Attribution-NonCommercial (http://creativecommons.org/licenses/by-nc/4.0/).</p>


## Prerequisites
* Just Blender 3.0 or higher!

## Features
* Automatically scales the modified mesh to align with Blender's coordinate system for easy visibility
* Control over the size of each instanced cube
* Unlike the Remesh modifier that comes with Blender, the Block-On add-on preserves the original Materials of the mesh, no need to reapply them later!
* View height cross-sections, helpful for procedural animations and creating a blueprint for creations in Minecraft or with Lego blocks

## How to use

* Make sure you're using Blender 3.0 or higher, as older versions do not have the necessary geometry nodes
* Download the file `block_on.py`. There are a couple different methods to do this on GitHub:
  1. **Good 'ole copy-paste:** copy the contents of the python file. Open up Blender and go to the scripting tab. Create a new script, and paste the file. From here you can save the file as `block_on.py` in your preferred location. 
  2. Open up the file in GitHub and click on the button that says Raw. From here, you right-click to save the page to your Downloads folder. If using Windows, switch the filetype from Text Document to All Files. Make sure it is named `block_on.py`, and save it.
  3. (Git) You can clone the entire repository if you like by running
        ```
        git clone https://github.com/BrendanParmer/Block-On
        ```
      in your terminal at your desired location. This method also makes updates easier, as you can simply pull from the origin whenever a new version of Block-On has been released.
* To enable the add-on, open Blender and go to `Edit > Preferences > Add-ons`, and click the Install button in the top-right. From there, navigate to where you downloaded the python file, and click on it to install
* Now that you have the Block-On add-on enabled, you can use it by selecting any mesh, and doing `Object > Block On`. This automatically generates a Geometry Nodes group for your object, with parameters you can adjust in the Modifiers tab
* If you have an **image texture** that you use in a material, open up its shader editor. Add an Attribute node, and use the attribute named Solidify Colors created by the modifier. Connect the Vector socket to the Vector socket of your image texture node, and it will automatically apply.

  <img 
        src="https://github.com/BrendanParmer/Block-On/blob/master/imgs/sc_mat.png" 
        alt="Solidify Colors Material" 
        width="960" 
        height="400">

## Parameters
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

## FAQs
**How do I update Block-On?**
* Block-On will continue to improve as new features are added, bugs are fixed, and new versions of Blender are release. To update Block-On, simply redownload the python file as new versions are released. Be sure to check back in whenever you update your version of Blender, as there may be new features. 

**Why aren't the cubes in the right location/orientation/size?**
* This is typically due to not applying transforms to the mesh. You can check in the Object Properties tab to see the transforms are default, and if not, press CTRL-A to apply All Transforms to the mesh. The add-on will update the mesh automatically

**How do I change the parameter limits?**
* Soft limits have been put in place to make it harder to accidentally crash Blender. However, if need be you can open the Block On node group in a Geometry Nodes tab and navigate to the Group panel. From there, you can see the list of inputs and outputs. Click on the input you want to modify, and you can specify your needed constraints for your project

**I added another material to my mesh, but it isn't updating. Why?**
* To keep a procedural workflow within the existing Geometry Nodes framework, the Block-On add-on needs to separate the faces of the mesh by all its materials when the node group is initialized. To do this, it loops through the existing materials in the mesh and creates a sub-node group for each one
* To update the mesh, reapply the Block-On function in the Object tag. The add-on will automatically remove the previous node groups from your blend file and replace them with new ones

**Why is the node group not generating?**
* Your version of Blender may be outdated. As of May 2022, support is officially available for Blender 3.0 and 3.1, as well as the 3.2 beta. 

**What are you planning on adding for the future?**
* I am currently working on better ways for users to download the add-on and be notified when new updates are released. This will probably look like a website where you can download the python file, read the documentation, and sign up for an email list for when new updates are released
* I also plan to add more advanced input options, like changing the resolution, level, and levels visible in each dimension, or using a different object for instancing than just the cube. I don't want the modifier to become too cluttered, so I'm thinking about how best to approach this
* I'd also like to add an option that allows a user to reduced a material down to a single color per cube to further achieve the stylized, blocky look. This is possible by using a Snap Vector Math node in a new material, and may be implemented soon
