from ctypes import *

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
            ("classes",POINTER(c_char_p)),
            ("parameters",POINTER(brv_brick_parameter)),
            ("bricks",POINTER(brv_brick))
            ]
