bl_info = {
    "name" : "Block On",
    "description" : "Script to voxelized 3D object and view its cross-sections",
    "author" : "Brendan Parmer",
    "version" : (0, 4, 0),
    "blender" : (2, 80, 0),
    "location" : "Object",
    "category" : "Object"
}

import bpy
import bmesh
import block_on as bo
import mathutils
import math
import time

class BlockOn(bpy.types.Operator):
    #Block On
    bl_idname = "object.block_on"
    bl_label = "Block On"
    bl_option = {'REGISTER', 'UNDO'}

    height: bpy.props.IntProperty(name = "Height", default = 6, min = 1, max = 8)

    def execute(self, context):
        start_time = time.time()
        
        height = 128
        depth = math.floor(math.log2(height)) + 1
        
        scene = context.scene
        orig_name = bpy.context.object.name
        orig_obj = bpy.data.objects[orig_name]
        view_layer = bpy.context.view_layer

        #duplicates object to keep original intact while we still have a destructive workflow
        bpy.ops.object.duplicate({"object" : orig_obj, "selected_objects" : [orig_obj]})
        bpy.context.object.name = orig_name + " Voxelized"
        bpy.ops.object.transform_apply(location = True, rotation = True, scale = False)
        bpy.ops.object.convert(target = 'MESH')


        blocky_name = bpy.context.object.name
        blocky_obj = bpy.data.objects[blocky_name]
        
        blocky_obj.select_set(True)
        view_layer.objects.active = blocky_obj

        orig_obj.hide_render = True
        orig_obj.hide_set(True)
        
        #make sure all faces are triangles
        bpy.ops.object.modifier_add(type='TRIANGULATE')
        bpy.ops.object.modifier_apply(modifier = 'Triangulate')
        
        #determine max dimension of object
        max = blocky_obj.dimensions.x
        if (blocky_obj.dimensions.y > max):
            max = blocky_obj.dimensions.y
        if (blocky_obj.dimensions.z > max):
            max = blocky_obj.dimensions.z  
        s_factor = height/max

        #voxelization
        bo.init(depth);
        mesh = blocky_obj.data
                       
        bm = bmesh.new()
        bm.from_mesh(mesh)
        
        i = 0
        for f in bm.faces:
            p0 = f.verts[0].co * s_factor
            p1 = f.verts[1].co * s_factor
            p2 = f.verts[2].co * s_factor  
            
            print("\n\nVoxelizing Triangle " + str(i) + ":")
            print("p0 = (" + str(round(p0.x, 3)) + ", " + 
                             str(round(p0.y, 3)) + ", " + 
                             str(round(p0.z, 3)) + ")")
                             
            print("p1 = (" + str(round(p1.x, 3)) + ", " + 
                             str(round(p1.y, 3)) + ", " + 
                             str(round(p1.z, 3)) + ")")
                             
            print("p2 = (" + str(round(p2.x, 3)) + ", " + 
                             str(round(p2.y, 3)) + ", " + 
                             str(round(p2.z, 3)) + ")\n")
                             
            bo.vox_tri(p0.x, p0.y, p0.z,
                       p1.x, p1.y, p1.z,
                       p2.x, p2.y, p2.z)
            print("Triangle " + str(i) + " voxelized")
            i += 1
            if (i > 1):
                break

        print("\nAll triangles voxelized\nRetrieving points from octree")
        points = bo.end()
        print("Points retrieved.\nAdding cubes...\n")
        
        for point in points:
            print("(" + str(point.x) + ", " + str(point.y) + ", " + str(point.z) + ")")
            transform = mathutils.Matrix.Translation((point.x + 0.5, point.y + 0.5, point.z + 0.5))
            bmesh.ops.create_cube(
                bm,
                1.0,
                transform,
                false
            )
        print("Cubes added to bmesh\n")
        bm.to_mesh(mesh)
        bm.free()
        print("Script finished execution.")
        print("It took %s seconds" % (time.time() - start_time))                     
        return {'FINISHED'}
    
def menu_func(self, context):
    self.layout.operator(BlockOn.bl_idname)
        
def register():
    bpy.utils.register_class(BlockOn)
    bpy.types.VIEW3D_MT_object.append(menu_func)
        
def unregister():
    bpy.utils.unregister_class(BlockOn)
    bpy.types.VIEW3D_MT_objects.remove(menu_func)
        
if __name__ == "__main__":
    register()
