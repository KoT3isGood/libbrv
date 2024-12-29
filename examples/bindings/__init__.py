from ctypes import *
import ctypes
import os
import bpy
from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty
from bpy.types import Operator
from bpy.props import StringProperty, BoolProperty, FloatProperty
import math
import time

class brv_brick_parameter(Structure):
    _fields_ = [
            ("name", c_char_p),
            ("datasize", c_uint),
            ("data", c_void_p),
            ("numparameters", c_int),
            ("size", c_short),
            ("sizes", POINTER(c_short)),
            ("offsets", POINTER(c_short)),
            ]


class brv_material(Structure):
    _fields_ = [
            ("color",c_float*3),
            ("alpha", c_float),
            ("pattern", c_char_p),
            ("material",c_char_p)
            ]


class brv_brick(Structure):
    pass


class brv_input(Structure):
    _fields_ = [
            ("mode", c_int),
            ("inputaxis", c_char_p),
            ("numsourcebricks", c_int),
            ("sourcebricks",POINTER(POINTER(brv_brick))),
            ("value", c_float),
            ]


class brv_output(Structure):
    _fields_ = [
            ("minin",c_float),
            ("maxin",c_float),
            ("minout",c_float),
            ("maxout",c_float),
            ]

class brv_wheel(Structure):
    _fields_ = [
            ("diameter",c_float),
            ("width", c_float),
            ]


brv_brick._fields_ = [
        ("next", POINTER(brv_brick)),
        ("name", c_char_p),
        ("type", c_int),
        ("numparameters", c_ubyte),
        ("parameters",POINTER(brv_brick_parameter)),
        ("material",brv_material),
        ("input",brv_input),
        ("output",brv_output),
        ("resetozero",c_char),
        ("operation",c_char_p),
        ("inputa",brv_input),
        ("inputb",brv_input),
        ("text",c_char_p),
        ("wheel",brv_wheel),
        ("size",c_float*3),
        ("position",c_float*3),
        ("rotation",c_float*3),
        ]



class brv_vehicle(Structure):
    _fields_ = [
            ("version",c_ubyte),
            ("numobjects",c_ushort),
            ("numclasses",c_ushort),
            ("numproperties", c_ushort),
            ("classes",POINTER(POINTER(c_char))),
            ("parameters",POINTER(brv_brick_parameter)),
            ("bricks",POINTER(brv_brick))
            ]


addon_path = os.path.dirname(__file__)
if os.name=="posix":
    lib = CDLL(os.path.join(addon_path,"libbrv.so"))
if os.name=="nt":
    lib = CDLL(os.path.join(addon_path,"brv.dll"))
lib.brv_read.argtypes = [POINTER(c_ubyte)]
lib.brv_read.restype = brv_vehicle
print(lib)
loaded_bricks = {}

wheel_diameter_lut = {}
wheel_diameter_lut["SprocketWheel"]=0.90
wheel_diameter_lut["IdlerWheel"]=0.90
wheel_diameter_lut["DragWheel_4x2"]=0.60
wheel_width_lut = {}
wheel_width_lut["SprocketWheel"]=0.3
wheel_width_lut["IdlerWheel"]=0.3

def import_new_brick(name):
    modelpath = "Bricks/SM_"+name+".gltf"
    modelpath=os.path.join(addon_path,modelpath)
    if not os.path.exists(modelpath):
        return 0

    try:
        bpy.ops.import_scene.gltf(filepath=modelpath)
    except:
        return 0
    imported_object = bpy.context.selected_objects[0]
    imported_object.name = name
    loaded_bricks[name]=imported_object
    return imported_object

def create_instance(name):
    instance = bpy.data.objects.new(name=name, object_data=loaded_bricks[name].data.copy())

    bpy.context.scene.collection.objects.link(instance)
    return instance

def getBrick(self, name):
    if name in loaded_bricks:
        return create_instance(name)
    else:
        return import_new_brick(name)



def read_brv(self, context, filepath):


    with open(filepath, 'rb') as file:
        content = file.read()
    vehicle = lib.brv_read(cast(content, POINTER(c_ubyte)))
    lib.brv_deserialize(pointer(vehicle),None)
    if vehicle.version>14 or vehicle.version<0:
        self.report({'ERROR'}, f"Error importing .brv file: version is not supported ({vehicle.version})")
        return {'CANCELLED'}

    shaderspath = "Bricks/shaders.blend"
    shaderspath=os.path.join(addon_path,shaderspath)

    wheel_node = "brv_wheel"
    with bpy.data.libraries.load(shaderspath, link=False) as (data_from, data_to):
        if wheel_node in data_from.node_groups:
            data_to.node_groups.append(wheel_node)

    # Now the node group is available in bpy.data.node_groups
    geo_node_group = bpy.data.node_groups[wheel_node]
    print(f"Loaded Geometry Node Group: {geo_node_group.name}")

    loaded_bricks.clear()
    bricks = vehicle.bricks
    i = 0
    print(self.import_materials)
    while bricks:
        i+=1
        # print(brick.contents.name)
        brick = bricks.contents
        bricks = brick.next

        model = getBrick(self, brick.name.decode("UTF-8"))
        if model==0:
            continue
        model.location.x = brick.position[0]
        model.location.y = brick.position[1]*-1
        model.location.z = brick.position[2]
       
        model.rotation_mode = "XYZ"
        model.rotation_euler.x = math.radians(brick.rotation[2])
        model.rotation_euler.y = math.radians(brick.rotation[0])*-1
        model.rotation_euler.z = math.radians(brick.rotation[1])*-1

        model.scale.x = brick.size[0]
        model.scale.y = brick.size[1]
        model.scale.z = brick.size[2]

        if self.import_materials:
            materialname = f"Plasic{i}"
            mat = bpy.data.materials.new(materialname)
            mat.diffuse_color = (brick.material.color[0], brick.material.color[1], brick.material.color[2], 1.0)
            model.active_material = mat
        if brick.type & 16:
            shader = model.modifiers.new(name="brv_wheel",type='NODES')
            shader.node_group=geo_node_group
            wd_node = shader.node_group.nodes.get("wheelradius")
            wd_node.outputs[0].default_value=(brick.wheel.diameter-wheel_diameter_lut[brick.name.decode("UTF-8")])/2
            wd_node = shader.node_group.nodes.get("wheelwidth")
            wd_node.outputs[0].default_value=(brick.wheel.width-wheel_width_lut[brick.name.decode("UTF-8")])


    loaded_bricks.clear()




    return {'FINISHED'}
class ImportBrv(Operator, ImportHelper):
    """Imports Brick Rigs Vehicles"""
    bl_idname = "import_brv.import"
    bl_label = "Import Brick Rigs Vehicle"
    filename_ext = ".brv"
    filter_glob = StringProperty(
            default="*.brv",
            options={'HIDDEN'},
            maxlen=255,
            )

    import_materials: BoolProperty(
            name="(Experimental) Enable material import",
            default=False
            )
    def execute(self, context):
        return read_brv(self, context, self.filepath)


def menu_func_import(self, context):
    self.layout.operator(ImportBrv.bl_idname, text="Brick Rigs Vehicle (.brv)")
def register():
    bpy.utils.register_class(ImportBrv)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)
def unregister():
    bpy.utils.unregister_class(ImportBrv)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)

bl_info = {
    "name": "brv improter",
    "author": "kotofyt",
    "description": "This Add-on allows to import .brv (Brick Rigs Vehicle) files",
    "blender": (4, 0, 0),
    "version": (1, 0, 0),
    "category": "Import-Export"
}
