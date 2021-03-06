bl_info = {
    "name" : "Block On",
    "description" : "Voxelize object and view its cross sections",
    "author" : "Brendan Parmer", 
    "version" : (1, 1, 0),
    "blender" : (3, 0, 0),
    "location" : "Object",
    "category" : "Object"
}
import bpy

class BlockOn(bpy.types.Operator):
    bl_idname = "object.block_on"
    bl_label = "Block On"
    bl_options = {'REGISTER', 'UNDO'}

    sc: bpy.props.BoolProperty(name="Solidify Colors", default=True, description="Creates a color attribute for object with image textures")
    bo: bpy.props.BoolProperty(name="Block On", default=True, description="Transforms and voxelizes geometry")
    
    def execute(self, context):
        #set up
        name = bpy.context.object.name
        obj = bpy.data.objects[name]
        
        #assign material if none exists
        if not obj.data.materials:
            mat = bpy.data.materials.get("Material")
            if mat is None:
                mat = bpy.data.materials.new(name="Material")
            obj.data.materials.append(mat)
        
        self.cleanup(obj, name) 
        
        if (self.sc):
            self.sc_modifier(obj, name)

        if (self.bo):
            self.bo_modifier(obj, name)
        
        return {"FINISHED"}

    """
    Deletes old Block On stuff if it exists
    """        
    def cleanup(self, obj, name):
        #solidify colors modifier
        old_sc_mod = obj.modifiers.get("Solidify Colors")
        if old_sc_mod is not None:
            obj.modifiers.remove(old_sc_mod)
            
        #block on modifier
        old_bo_mod = obj.modifiers.get("Block On")
        if old_bo_mod is not None:
            obj.modifiers.remove(old_bo_mod)
           
        #node groups
        block_on_groups = ["Block On ", 
                           "Generate Cubes ", 
                           "Level Viewer ", 
                           "Material ", 
                           "On Points ",
                           "Transform Mesh ",
                           "Voxelize ",
                           "Solidify Colors "]
        node_groups = bpy.data.node_groups
        
        for group_name in block_on_groups:
            group = node_groups.get(group_name + name)
            if group is not None:
                node_groups.remove(group)

    """
    Creates Solidify Colors modifier
    """
    def sc_modifier(self, obj, name):
        if obj.data.attributes.get("Solidify Colors") is None:
                obj.data.attributes.new(name="Solidify Colors", type='FLOAT_COLOR', domain='POINT')
        
        node_groups = bpy.data.node_groups
        sc_attr_name = "Solidify Colors"
        
        self.solidify_colors_node_group(name)
        sc_modifier = obj.modifiers.new(name = sc_attr_name, type = "NODES")
        if bpy.app.version < (3, 2, 0):
            node_groups.remove(sc_modifier.node_group)
        sc_modifier.node_group = node_groups[sc_attr_name + " " + name]
    
        input_id = next(i for i in sc_modifier.node_group.inputs if i.name == "UV Map").identifier
        sc_modifier[input_id + "_attribute_name"] = "UVMap"
        sc_modifier[input_id + "_use_attribute"]  = True
        
        output_id = next(i for i in sc_modifier.node_group.outputs if i.name == "Color").identifier
        sc_modifier[output_id + "_attribute_name"] = sc_attr_name
        sc_modifier[output_id + "_use_attribute"]  = True

    """
    Creates Block On modifier
    """
    def bo_modifier(self, obj, name):
        #block on modifier
        node_groups = bpy.data.node_groups
        self.block_on_node_group(obj_name = name)
        bo_modifier = obj.modifiers.new(name = "Block On", type = "NODES")
        if bpy.app.version < (3, 2, 0):
            node_groups.remove(bo_modifier.node_group)
        bo_modifier.node_group = node_groups["Block On " + name]

    """
    Create main Block On geometry node group
    """       
    def block_on_node_group(self, obj_name):
        bo = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "Block On " + obj_name)

        #input node
        bo.inputs.new("NodeSocketGeometry", "Geometry")
        bo.inputs.new("NodeSocketInt",      "Resolution")
        bo.inputs.new("NodeSocketFloat",    "Cube Size")
        bo.inputs.new("NodeSocketInt",      "Level")
        bo.inputs.new("NodeSocketInt",      "Levels Visible")
        bo.inputs.new("NodeSocketFloat",    "Density")
        bo.inputs.new("NodeSocketInt",      "Seed")
        bo.inputs.new("NodeSocketBool",     "On Points")

        bo_input = bo.nodes.new("NodeGroupInput")
        bo_input.location = (0,0)

        #Transform Mesh
        self.transform_mesh_node_group(obj_name)
        bo_tm = bo.nodes.new("GeometryNodeGroup")
        bo_tm.location = (200, 250)
        
        bo_tm.node_tree = bpy.data.node_groups["Transform Mesh " + obj_name]
        
        bo.links.new(bo_input.outputs["Geometry"],   bo_tm.inputs["Geometry"])
        bo.links.new(bo_input.outputs["Resolution"], bo_tm.inputs["Resolution"])
        
        #instance cube
        bo_cube = bo.nodes.new("GeometryNodeMeshCube")
        bo_cube.location = (200, 100)
        bo.links.new(bo_input.outputs["Cube Size"], bo_cube.inputs["Size"])
        
        #Generate Cubes
        self.generate_cubes_node_group(obj_name)
        bo_gc = bo.nodes.new("GeometryNodeGroup")
        bo_gc.location = (400, 50)
        
        bo_gc.node_tree = bpy.data.node_groups["Generate Cubes " + obj_name]
        
        bo.links.new(bo_tm.outputs["Geometry"],          bo_gc.inputs["Mesh"])
        bo.links.new(bo_cube.outputs["Mesh"],            bo_gc.inputs["Instance"])
        bo.links.new(bo_input.outputs["Level"],          bo_gc.inputs["Level"])
        bo.links.new(bo_input.outputs["Levels Visible"], bo_gc.inputs["Levels Visible"])
        bo.links.new(bo_input.outputs["Density"],        bo_gc.inputs["Density"])
        bo.links.new(bo_input.outputs["Seed"],           bo_gc.inputs["Seed"])
        
        #On Points
        self.on_points_node_group(obj_name)
        bo_op = bo.nodes.new("GeometryNodeGroup")
        bo_op.location = (600, -100)
        
        bo_op.node_tree = bpy.data.node_groups["On Points " + obj_name]
        
        bo.links.new(bo_gc.outputs["Geometry"],     bo_op.inputs["Geometry"])
        bo.links.new(bo_input.outputs["On Points"], bo_op.inputs["On Points"])
        
        #output node
        bo.outputs.new("NodeSocketGeometry", "Geometry")
        
        bo_output = bo.nodes.new("NodeGroupOutput")
        bo_output.location = (800, 100)
        bo.links.new(bo_op.outputs["Geometry"], bo_output.inputs["Geometry"])
        
        #set defaults
        DEFAULT_RES = 32
        MAX_RES = 256
        
        bo.inputs["Resolution"].min_value = 1
        bo.inputs["Resolution"].default_value = DEFAULT_RES
        bo.inputs["Resolution"].max_value = MAX_RES
        
        bo.inputs["Cube Size"].min_value = 0.0
        bo.inputs["Cube Size"].default_value = 0.9
        bo.inputs["Cube Size"].max_value = 1.0
        
        bo.inputs["Level"].min_value = 0
        bo.inputs["Level"].default_value = 0
        bo.inputs["Level"].max_value = MAX_RES
        
        bo.inputs["Levels Visible"].min_value = 0
        bo.inputs["Levels Visible"].default_value = DEFAULT_RES
        bo.inputs["Levels Visible"].max_value = MAX_RES
        
        bo.inputs["Density"].min_value = 0.0
        bo.inputs["Density"].default_value = 10.0
        bo.inputs["Density"].max_value = 100.0

    """
    Creates geometry node group that scales and aligns the mesh so each block is one unit wide
    """   
    def transform_mesh_node_group(self, set_name):
        tm = bpy.data.node_groups.new(type="GeometryNodeTree", name = "Transform Mesh " + set_name)
        
        #input node
        tm.inputs.new("NodeSocketGeometry", "Geometry")
        tm.inputs.new("NodeSocketInt",      "Resolution")
        
        tm_input = tm.nodes.new("NodeGroupInput")
        tm_input.location = (0,0)

        #position
        tm_pos = tm.nodes.new("GeometryNodeInputPosition")
        tm_pos.location = (-200, -200)
        tm_pos.hide = True
        
        """
        There is an easier way to do this with nodes, but for some reason I can't connect the position and attribute statistic vectors with scripting, so I've gotta separate by XYZ. 
        """
        #separate xyz
        tm_sep_xyz = tm.nodes.new("ShaderNodeSeparateXYZ")
        tm_sep_xyz.location = (0, -200)
        tm_sep_xyz.hide = True
        tm.links.new(tm_pos.outputs["Position"], tm_sep_xyz.inputs[0])
        
        #attribute statistic x
        tm_as_x = tm.nodes.new("GeometryNodeAttributeStatistic")
        tm_as_x.location = (200, -150)
        tm_as_x.hide = True
        
        tm.links.new(tm_input.outputs["Geometry"], tm_as_x.inputs["Geometry"])
        tm.links.new(tm_sep_xyz.outputs["X"],      tm_as_x.inputs["Attribute"])
        
        #attribute statistic y
        tm_as_y = tm.nodes.new("GeometryNodeAttributeStatistic")
        tm_as_y.location = (200, -225)
        tm_as_y.hide = True
        
        tm.links.new(tm_input.outputs["Geometry"], tm_as_y.inputs["Geometry"])
        tm.links.new(tm_sep_xyz.outputs["Y"],      tm_as_y.inputs["Attribute"])
        
        #attribute statistic z
        tm_as_z = tm.nodes.new("GeometryNodeAttributeStatistic")
        tm_as_z.location = (200, -300)
        tm_as_z.hide = True
        
        tm.links.new(tm_input.outputs["Geometry"], tm_as_z.inputs["Geometry"])
        tm.links.new(tm_sep_xyz.outputs["Z"],      tm_as_z.inputs["Attribute"])
        
        #max one
        tm_max_1 = tm.nodes.new("ShaderNodeMath")
        tm_max_1.location = (400, -200)
        tm_max_1.hide = True
        
        tm_max_1.operation = "MAXIMUM"
        
        tm.links.new(tm_as_x.outputs["Range"], tm_max_1.inputs[0])
        tm.links.new(tm_as_y.outputs["Range"], tm_max_1.inputs[1])
        
        #max two
        tm_max_2 = tm.nodes.new("ShaderNodeMath")
        tm_max_2.location = (600, -200)
        tm_max_2.hide = True
        
        tm_max_2.operation = "MAXIMUM"
        
        tm.links.new(tm_max_1.outputs[0],      tm_max_2.inputs[0])
        tm.links.new(tm_as_z.outputs["Range"], tm_max_2.inputs[1])
        
        #divide
        tm_div = tm.nodes.new("ShaderNodeMath")
        tm_div.location = (800, -200)
        tm_div.hide = True
        
        tm_div.operation = "DIVIDE"
        
        tm.links.new(tm_input.outputs["Resolution"], tm_div.inputs[0])
        tm.links.new(tm_max_2.outputs[0], tm_div.inputs[1])
        
        #scalar multiply
        tm_mul = tm.nodes.new("ShaderNodeMath")
        tm_mul.location = (1000, -100)
        
        tm_mul.operation = "MULTIPLY"
        
        tm.links.new(tm_div.outputs[0], tm_mul.inputs[0])
        tm_mul.inputs[1].default_value = -1.0
        
        #min vector
        tm_min = tm.nodes.new("ShaderNodeCombineXYZ")
        tm_min.location = (1000, -300)
        tm_min.hide = True
        
        tm.links.new(tm_as_x.outputs["Min"], tm_min.inputs["X"])
        tm.links.new(tm_as_y.outputs["Min"], tm_min.inputs["Y"])
        tm.links.new(tm_as_z.outputs["Min"], tm_min.inputs["Z"])
        
        #vector multiply
        tm_vec_mul = tm.nodes.new("ShaderNodeVectorMath")
        tm_vec_mul.location = (1200, -250)
        tm_vec_mul.hide = True
        
        tm_vec_mul.operation = "MULTIPLY"
        
        tm.links.new(tm_mul.outputs[0], tm_vec_mul.inputs[0])
        tm.links.new(tm_min.outputs[0], tm_vec_mul.inputs[1])
        
        #vector add
        tm_vec_add = tm.nodes.new("ShaderNodeVectorMath")
        tm_vec_add.location = (1400, -150)
        
        tm.links.new(tm_vec_mul.outputs[0], tm_vec_add.inputs[0])
        tm_vec_add.inputs[1].default_value = (1.0, 1.0, 1.0)
        
        #transform
        tm_transform = tm.nodes.new("GeometryNodeTransform")
        tm_transform.location = (1600, -200)
        
        tm.links.new(tm_input.outputs["Geometry"], tm_transform.inputs["Geometry"])
        tm.links.new(tm_vec_add.outputs[0],        tm_transform.inputs["Translation"])
        tm.links.new(tm_div.outputs[0],            tm_transform.inputs["Scale"])
        
        #output node
        tm.outputs.new("NodeSocketGeometry", "Geometry")
        
        tm_output = tm.nodes.new("NodeGroupOutput")
        tm_output.location = (1800,-200)
        tm.links.new(tm_transform.outputs["Geometry"], tm_output.inputs["Geometry"])

    """
    Creates the node group that performs voxelization and material management
    """
    def generate_cubes_node_group(self, set_name):
        gc = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "Generate Cubes " + set_name)
        
        #input node
        gc.inputs.new("NodeSocketGeometry", "Mesh")
        gc.inputs.new("NodeSocketGeometry", "Instance")
        gc.inputs.new("NodeSocketInt",      "Level")
        gc.inputs.new("NodeSocketInt",      "Levels Visible")
        gc.inputs.new("NodeSocketFloat",    "Density")
        gc.inputs.new("NodeSocketInt",      "Seed")
        
        gc_input = gc.nodes.new("NodeGroupInput")
        gc_input.location = (0,0)
        
        #join geometry
        gc_join = gc.nodes.new("GeometryNodeJoinGeometry")
        gc_join.location = (400, 0)
        
        #materials
        self.material_node_group(set_name)
        obj = bpy.data.objects[set_name]
        for i, m in enumerate(obj.material_slots):
            material = m.material 
            gc_m = gc.nodes.new("GeometryNodeGroup")
            y_pos =  -(len(obj.material_slots) * 30 - i * 60)
            gc_m.location = (200, y_pos)
            gc_m.hide = True
            
            gc_m.node_tree = bpy.data.node_groups["Material " + set_name]
            gc_m.inputs["Material"].default_value = material
            
            #inputs
            gc.links.new(gc_input.outputs["Mesh"],           gc_m.inputs["Mesh"])
            gc.links.new(gc_input.outputs["Instance"],       gc_m.inputs["Instance"])
            gc.links.new(gc_input.outputs["Level"],          gc_m.inputs["Level"])
            gc.links.new(gc_input.outputs["Levels Visible"], gc_m.inputs["Levels Visible"])
            gc.links.new(gc_input.outputs["Density"],        gc_m.inputs["Density"])
            gc.links.new(gc_input.outputs["Seed"],           gc_m.inputs["Seed"])
            
            #output
            gc.links.new(gc_m.outputs["Geometry"], gc_join.inputs[0])
        
        #output node
        gc.outputs.new("NodeSocketGeometry", "Geometry")
        
        gc_output = gc.nodes.new("NodeGroupOutput")
        gc_output.location = (600, 0)
        
        gc.links.new(gc_join.outputs["Geometry"], gc_output.inputs["Geometry"])

    """
    Creates the node group that handles objects with multiple materials
    """   
    def material_node_group(self, set_name):
        m = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "Material " + set_name)
        
        #input node
        m.inputs.new("NodeSocketGeometry", "Mesh")
        m.inputs.new("NodeSocketGeometry", "Instance")
        m.inputs.new("NodeSocketInt",      "Level")
        m.inputs.new("NodeSocketInt",      "Levels Visible")
        m.inputs.new("NodeSocketFloat",    "Density")
        m.inputs.new("NodeSocketInt",      "Seed")
        m.inputs.new("NodeSocketMaterial", "Material")
        
        m_input = m.nodes.new("NodeGroupInput")
        m_input.location = (0, 0)
        
        #material selection
        m_mat_sel = m.nodes.new("GeometryNodeMaterialSelection")
        m_mat_sel.location = (200, 0)
        
        m.links.new(m_input.outputs["Material"], m_mat_sel.inputs["Material"])
        
        #distribute points on faces
        m_dpof = m.nodes.new("GeometryNodeDistributePointsOnFaces")
        m_dpof.location = (400, 0)
        
        m.links.new(m_input.outputs["Mesh"],        m_dpof.inputs["Mesh"])
        m.links.new(m_mat_sel.outputs["Selection"], m_dpof.inputs["Selection"])
        m.links.new(m_input.outputs["Density"],     m_dpof.inputs["Density"])
        m.links.new(m_input.outputs["Seed"],        m_dpof.inputs["Seed"])
        
        #voxelize
        self.voxelize_node_group(set_name)

        m_v = m.nodes.new("GeometryNodeGroup")
        m_v.location = (600, 0)
        
        m_v.node_tree = bpy.data.node_groups["Voxelize " + set_name]
        
        m.links.new(m_dpof.outputs["Points"], m_v.inputs["Geometry"])
        
        #level visualizer
        self.level_viewer_node_group(set_name)
        
        m_lv = m.nodes.new("GeometryNodeGroup")
        m_lv.location = (600, -200)
        
        m_lv.node_tree = bpy.data.node_groups["Level Viewer " + set_name]
        
        m.links.new(m_input.outputs["Level"],          m_lv.inputs["Level"])
        m.links.new(m_input.outputs["Levels Visible"], m_lv.inputs["Levels Visible"])
        
        #instance on points
        m_iop = m.nodes.new("GeometryNodeInstanceOnPoints")
        m_iop.location = (800, 0)
        
        m.links.new(m_v.outputs["Geometry"],     m_iop.inputs["Points"])
        m.links.new(m_lv.outputs[0],             m_iop.inputs["Selection"])
        m.links.new(m_input.outputs["Instance"], m_iop.inputs["Instance"])
        
        #realize instances
        m_ri = m.nodes.new("GeometryNodeRealizeInstances")
        m_ri.location = (1000, 0)
        
        m.links.new(m_iop.outputs["Instances"], m_ri.inputs["Geometry"])
        
        #set material
        m_sm = m.nodes.new("GeometryNodeSetMaterial")
        m_sm.location = (1200, 0)

        m.links.new(m_ri.outputs["Geometry"],    m_sm.inputs["Geometry"])
        m.links.new(m_input.outputs["Material"], m_sm.inputs["Material"])

        #output node
        m.outputs.new("NodeSocketGeometry", "Geometry")

        m_output = m.nodes.new("NodeGroupOutput")
        m_output.location = (1400, 0)

        m.links.new(m_sm.outputs["Geometry"], m_output.inputs["Geometry"])

    """
    Creates the node group responsible for voxelization
    """ 
    def voxelize_node_group(self, set_name):
        v = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "Voxelize " + set_name)
        
        #input node
        v.inputs.new("NodeSocketGeometry", "Geometry")
        
        v_input = v.nodes.new("NodeGroupInput")
        v_input.location = (0, 0)
        
        #position
        v_pos = v.nodes.new("GeometryNodeInputPosition")
        v_pos.location = (0, -100)
        
        #snap
        v_snap = v.nodes.new("ShaderNodeVectorMath")
        v_snap.location = (200, -100)
        
        v_snap.operation = "SNAP"
        
        v.links.new(v_pos.outputs["Position"], v_snap.inputs[0])
        v_snap.inputs[1].default_value = (1.0, 1.0, 1.0)
        
        #subtract
        v_sub = v.nodes.new("ShaderNodeVectorMath")
        v_sub.location = (400, -100)
        
        v_sub.operation = "SUBTRACT"
        
        v.links.new(v_snap.outputs[0],         v_sub.inputs[0])
        v.links.new(v_pos.outputs["Position"], v_sub.inputs[1])
        
        #set position
        v_sp = v.nodes.new("GeometryNodeSetPosition")
        v_sp.location = (600, 0)
        
        v.links.new(v_input.outputs["Geometry"], v_sp.inputs["Geometry"])
        v.links.new(v_pos.outputs["Position"],   v_sp.inputs["Position"])
        v.links.new(v_sub.outputs[0],            v_sp.inputs["Offset"])
        
        #output node
        v.outputs.new("NodeSocketGeometry", "Geometry")
        
        v_output = v.nodes.new("NodeGroupOutput")
        v_output.location = (800, 0)
        
        v.links.new(v_sp.outputs["Geometry"], v_output.inputs["Geometry"])

    """
    Creates the node group that adjusts which levels are visible
    """ 
    def level_viewer_node_group(self, set_name):
        lv = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "Level Viewer " + set_name)
        
        #input node
        lv.inputs.new("NodeSocketInt", "Level")
        lv.inputs.new("NodeSocketInt", "Levels Visible")
        
        lv_input = lv.nodes.new("NodeGroupInput")
        lv_input.location = (0, 0)
       
        #position
        lv_pos = lv.nodes.new("GeometryNodeInputPosition")
        lv_pos.location = (200, -200)
        
        #separate xyz
        lv_sep = lv.nodes.new("ShaderNodeSeparateXYZ")
        lv_sep.location = (400, -200)
        
        lv.links.new(lv_pos.outputs["Position"], lv_sep.inputs[0])
        
        #add one
        lv_add_one = lv.nodes.new("ShaderNodeMath")
        lv_add_one.location = (200, 0)
        
        lv.links.new(lv_input.outputs["Levels Visible"], lv_add_one.inputs[0])
        lv_add_one.inputs[1].default_value = 1.0
        
        #add levels
        lv_add_lv = lv.nodes.new("ShaderNodeMath")
        lv_add_lv.location = (400, 0)
        
        lv.links.new(lv_input.outputs["Level"], lv_add_lv.inputs[0])
        lv.links.new(lv_add_one.outputs[0],     lv_add_lv.inputs[1])
        
        # lte
        lv_lte = None
        if bpy.app.version >= (3, 1, 0):
            lv_lte = lv.nodes.new("FunctionNodeCompare")
            lv_lte.location = (600, -200)

            lv_lte.operation = "LESS_EQUAL"

            lv.links.new(lv_sep.outputs["Z"],       lv_lte.inputs["A"])
            lv.links.new(lv_input.outputs["Level"], lv_lte.inputs["B"])
        elif bpy.app.version >= (3, 0, 0) and bpy.app.version < (3, 1, 0):
            lv_lte = lv.nodes.new("FunctionNodeCompareFloats")
            lv_lte.location = (600, -200)
            
            lv_lte.operation = "LESS_EQUAL"
            
            lv.links.new(lv_sep.outputs["Z"],       lv_lte.inputs["A"])
            lv.links.new(lv_input.outputs["Level"], lv_lte.inputs["B"])
        else:
            print("Sorry, your version of Blender is too old for Block-On to run")
        
        #gte
        lv_gte = None
        if bpy.app.version >= (3, 1, 0):
            lv_gte = lv.nodes.new("FunctionNodeCompare")
            lv_gte.location = (600, -400)
            
            lv_gte.operation = "GREATER_EQUAL"
            
            lv.links.new(lv_sep.outputs["Z"],  lv_gte.inputs["A"])
            lv.links.new(lv_add_lv.outputs[0], lv_gte.inputs["B"])
        elif bpy.app.version >= (3, 0, 0) and bpy.app.version < (3, 1, 0):
            lv_gte = lv.nodes.new("FunctionNodeCompareFloats")
            lv_gte.location = (600, -400)
            
            lv_gte.operation = "GREATER_EQUAL"
            
            lv.links.new(lv_sep.outputs["Z"],  lv_gte.inputs["A"])
            lv.links.new(lv_add_lv.outputs[0], lv_gte.inputs["B"])
        else:
            print("Sorry, your version of Blender is too old for Block-On to run")
        
        #or
        lv_or = lv.nodes.new("FunctionNodeBooleanMath")
        lv_or.location = (800, -300)
        
        lv_or.operation = "OR"
        
        lv.links.new(lv_lte.outputs[0], lv_or.inputs[0])
        lv.links.new(lv_gte.outputs[0], lv_or.inputs[1])
        
        #not
        lv_not = lv.nodes.new("FunctionNodeBooleanMath")
        lv_not.location = (1000, -300)
        
        lv_not.operation = "NOT"
        
        lv.links.new(lv_or.outputs[0], lv_not.inputs[0])
        
        
        #output node
        lv.outputs.new("NodeSocketBool", "Selection")
        
        lv_output = lv.nodes.new("NodeGroupOutput")
        lv_output.location = (1200, -300)
        lv.links.new(lv_not.outputs[0], lv_output.inputs[0])

    """
    Creates the node group that aligns blocks in between lines or on intersections
        0 - cubes will spawn on points like (0.5, 0.5, 0.5)
        1 - cubes will spawn on points like (1.0, 0.0, 0.0)
    """
    def on_points_node_group(self, set_name):
        op = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "On Points " + set_name)
        
        #input node
        op.inputs.new("NodeSocketGeometry", "Geometry")
        op.inputs.new("NodeSocketBool",     "On Points")
        
        op_input = op.nodes.new("NodeGroupInput")
        op_input.location = (0, 0)
        
        #not
        op_not = op.nodes.new("FunctionNodeBooleanMath")
        op_not.location = (200, -100)
        
        op_not.operation = "NOT"
        
        op.links.new(op_input.outputs["On Points"], op_not.inputs[0])
        
        #multiply
        op_mul = op.nodes.new("ShaderNodeMath")
        op_mul.location = (400, -100)
        
        op_mul.operation = "MULTIPLY"
        
        op.links.new(op_not.outputs[0], op_mul.inputs[0])
        op_mul.inputs[1].default_value = -0.5
        
        #transform
        op_transform = op.nodes.new("GeometryNodeTransform")
        op_transform.location = (600, 0)
        
        op.links.new(op_input.outputs["Geometry"], op_transform.inputs["Geometry"])
        op.links.new(op_mul.outputs[0],            op_transform.inputs["Translation"])
        
        #output node
        op.outputs.new("NodeSocketGeometry", "Geometry")
        
        op_output = op.nodes.new("NodeGroupOutput")
        op_output.location = (800, 0)
        
        op.links.new(op_transform.outputs["Geometry"], op_output.inputs["Geometry"])   

    """
    Creates the node group that creates a color attribute that solidifies colors from an image texture
    """    
    def solidify_colors_node_group(self, set_name):
        sc = bpy.data.node_groups.new(type = "GeometryNodeTree", name = "Solidify Colors " + set_name)
        
        #input node
        sc.inputs.new("NodeSocketGeometry", "Geometry")
        sc.inputs.new("NodeSocketColor",    "UV Map")
        
        sc_input = sc.nodes.new("NodeGroupInput")
        sc_input.location = (0, 0)
        
        #transfer attribute
        sc_ta = sc.nodes.new("GeometryNodeAttributeTransfer")
        
        sc_ta.data_type = 'FLOAT_VECTOR'
        sc_ta.mapping   = 'NEAREST'
        sc_ta.domain    = 'CORNER'
        
        sc_ta.location = (200, -100)
        
        sc.links.new(sc_input.outputs["Geometry"], sc_ta.inputs["Source"])
        sc.links.new(sc_input.outputs["UV Map"], sc_ta.inputs["Attribute"])
        
        #output node
        sc.outputs.new("NodeSocketGeometry", "Geometry")
        sc.outputs.new("NodeSocketColor",    "Color")
        sc.outputs["Color"].attribute_domain = 'FACE'
        
        sc_output = sc.nodes.new("NodeGroupOutput")
        sc_output.location = (400, 0)
        
        sc.links.new(sc_input.outputs["Geometry"], sc_output.inputs["Geometry"])
        sc.links.new(sc_ta.outputs["Attribute"],   sc_output.inputs["Color"])


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
