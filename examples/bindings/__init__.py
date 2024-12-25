import bpy
import os
from . import bindings
from bindings import *
from ctypes import *

from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty
from bpy.types import Operator

addon_path = os.path.dirname(__file__)
my_lib = CDLL(os.path.join(addon_path,"libbrv.so"))

bl_info = {
    "name": "brv improter",
    "author": "kotofyt",
    "description": "This Add-on allows to import .brv (Brick Rigs Vehicle) files",
    "blender": (4, 0, 0),
    "version": (1, 0, 0),
    "category": "Import-Export"
}



def process_brv(context, directory, files):
    import os
    for file in files:
        path = os.path.join(directory, file.name)
        print("process %s" % path)
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

    def execute(self, context):
        process_brv(context, self.directory, self.files)


def menu_func_import(self, context):
    self.layout.operator(ImportBrv.bl_idname, text="Brick Rigs Vehicle (.brv)")


def register():
    bpy.utils.register_class(ImportBrv)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportBrv)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
