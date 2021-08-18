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

class BlockOn(bpy.types.Operator):
    #Block On
    bl_idname = "object.block_on"
    bl_label = "Block On"
    bl_option = {'REGISTER', 'UNDO'}

    height: bpy.props.IntProperty(name = "Height", default = 6, min = 1, max = 8)

    def execute(self, context):
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
        
        #voxelization
        bo.init(6); #should be height but testing
        mesh = blocky_obj.data
        """mesh.calc_loop_triangles()
        for tri in mesh.loop_triangles:
            p0 = mesh.vertices[tri.vertices[0]].co
            p1 = mesh.vertices[tri.vertices[1]].co
            p2 = mesh.vertices[tri.vertices[2]].co
            bo.vox_tri(p0.x, p0.y, p0.z,
                       p1.x, p1.y, p1.z,
                       p2.x, p2.y, p2.z)"""
                       
        bm = bmesh.new()
        bm.from_mesh(mesh)
        
        print("Oh boy here we go")
        
        i = 0
        for f in bm.faces:
            p0 = f.verts[0].co
            p1 = f.verts[1].co
            p2 = f.verts[2].co    
            
            if i == 952 or i == 568 or i == 967:
                print("p0 = (" + str(p0.x) + ", " + str(p0.y) + ", " + str(p0.z) + ")")
                print("p1 = (" + str(p1.x) + ", " + str(p1.y) + ", " + str(p1.z) + ")")
                print("p2 = (" + str(p2.x) + ", " + str(p2.y) + ", " + str(p2.z) + ")")
            bo.vox_tri(p0.x, p0.y, p0.z,
                       p1.x, p1.y, p1.z,
                       p2.x, p2.y, p2.z)
            print("Triangle " + str(i) + " voxelized")
            i += 1
            print("Voxelizing Triangle " + str(i))
                             
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
