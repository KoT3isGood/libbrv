#pragma once
// you can use #define BRV_NO_DESERIALIZATION
// to use brv format as format not only for br vehicles

// from Source/BrickRigs/BrickEditor/BrickEditorSaveVersion.h
// Current save version, the legacy UBrickStatics version ended at 6
const static FBrickRigsSaveVersion BR_SAVE_VERSION = 15;
// Version where the brick units and brick size types were removed
const static FBrickRigsSaveVersion BR_SAVE_BRICK_UNITS_REMOVED_VERSION = 15;
// Version where the driver seat stopped being serialized
const static FBrickRigsSaveVersion BR_SAVE_DRIVER_SEAT_BY_INDEX_VERSION = 15;
// Version where color was saved as RGB again instead of HSV
const static FBrickRigsSaveVersion BR_SAVE_COLOR_RGB_VERSION = 15;
// Version where brick units started to be saved as a float instead of uint16
const static FBrickRigsSaveVersion BR_SAVE_BRICK_UNITS_FLOAT_VERSION = 14;
// Version where wheel meshes were moved so their connectors would line up with the origin
const static FBrickRigsSaveVersion BR_SAVE_ALIGNED_WHEEL_ORIGIN_VERSION = 14;
// Prior to this version GetNameStringByIndex was used instead of GetNameStringByIndex in FEnumBrickPropertyBase::SerializeProperty
const static FBrickRigsSaveVersion BR_SAVE_FIXED_ENUM_SERIALIZATION_VERSION = 13;
// Version where input channel indices where replaced by brick references
const static FBrickRigsSaveVersion BR_SAVE_INPUT_CHANNEL_REFERENCE_VERSION = 13;
// Version where item mass and price started to be saved to the meta data
const static FBrickRigsSaveVersion BR_SAVE_MASS_AND_PRICE = 12;
// Version where min and max sensor input values were replaced by universal input channel parameters
const static FBrickRigsSaveVersion BR_SAVE_INPUT_CHANNEL_CURVE_VERSION = 11;
// Version where the min and max actuation limit properties have been inverted
const static FBrickRigsSaveVersion BR_SAVE_ACTUATOR_LIMITS_VERSION = 10;
// Version where suspension damping was made player controllable
const static FBrickRigsSaveVersion BR_SAVE_SUSPENSION_DAMPING_VERSION = 9;
// Version where the tags were added to the meta data
const static FBrickRigsSaveVersion BR_SAVE_TAGS_VERSION = 8;
// Version where we transition from the UBrickStatics save functions to the UBrickEditorSaveInterface
const static FBrickRigsSaveVersion BR_SAVE_INTERFACE_VERSION = 7;
// Version before the save interface was implemented
const static FBrickRigsSaveVersion BR_SAVE_LAST_LEGACY_VERSION = 6;
// Version where input channels were introduced and the input axes enum has been changed
const static FBrickRigsSaveVersion BR_SAVE_INPUT_CHANNEL_VERSION = 6;
// Legacy version where the brick location accuracy was increased
const static FBrickRigsSaveVersion BR_SAVE_SMALLER_STEPS_VERSION = 4;
// Version where the saved element size has been fixed
const static FBrickRigsSaveVersion BR_SAVE_FIXED_ELEMENT_SIZE_VERSION = 3;

// Legacy version where the brick location accuracy was increased
// kotofyt: not sure where it is used since v6 uses v3's
#define BR_SAVE_SMALLER_STEPS_VERSION 4
// Version where the saved element size has been fixed
#define BR_SAVE_FIXED_ELEMENT_SIZE_VERSION 3

typedef enum brv_input_modes {
  BRV_INPUT_AXIS=0,
  BRV_INPUT_SOURCE_BRICKS=1,
  BRV_INPUT_VALUE=2,
} brv_input_modes;

typedef enum brv_brick_type {
  BRV_TYPE_SCALABLE_BRICK=1,
  BRV_TYPE_INPUT=2,
  BRV_TYPE_MATH_BRICK=4,
  BRV_TYPE_TEXT=8,
  BRV_TYPE_WHEEL=16
} brv_brick_type;
// raw brick parameter
typedef struct {
  char* name;
  unsigned int datasize;
  void* data;

  // v7+ may host params in itself
  // they wont be included inside of legacy or 
  int numparameters;
  short size;
  short* sizes;
  short* offsets;


} brv_brick_parameter;

typedef struct brv_material {
  float color[3];
  float alpha;
  char* pattern;
  char* material;
} material;

typedef struct brv_input {

  int mode;

  const char* inputaxis;
  int numsourcebricks;
  struct brv_brick** sourcebricks;
  float value;
} brv_input;

typedef struct brv_output {
  float minin;
  float maxin;
  float minout;
  float maxout;
} brv_output;

typedef struct brv_wheel {
  float diameter;
  float width;
} brv_wheel;

typedef struct brv_brick {
  struct brv_brick* next;
  char* name;

  unsigned char numparameters;
  brv_brick_parameter* parameters;

#ifndef BRV_NO_DESERIALIZATION
  int type;
  // materials
  struct brv_material material;

  // for switches
  brv_input input;
  brv_output output;
  char resettozero;

  // math bricks
  char* operation;
  brv_input inputa;
  brv_input inputb;

  // text bricks
  char* text;

  // wheels
  brv_wheel wheel;

  // scalable bricks, wheels, displays etc.
  float size[3];
#endif
  float position[3];
  float rotation[3];
} brv_brick;

#ifdef BRV_NO_DESERIALIZATION
typedef brv_brick brv_object;
typedef brv_brick_parameter brv_parameter;
#endif


typedef struct {
  unsigned char version;
  unsigned short numobjects; 
  unsigned short numclasses; 
  unsigned short numproperties;

  // intermediate information from BR_SAVE_INTERFACE_VERSION+ 
  char** classes;
  brv_brick_parameter* parameters;

  brv_brick* bricks;

} brv_vehicle;

typedef void(*brv_serialize_callback)(const char*, int, void*);
typedef void(*brv_deserialize_callback)(brv_brick_parameter*);

void libbrv_init(const char** dynamically_sized_properties, unsigned int num);
// reads raw brv and gets all bricks and properties
brv_vehicle brv_read(unsigned char* contents);

#ifndef BRV_NO_DESERIALIZATION 
// converts properties into brick parameters
void brv_deserialize(brv_vehicle* vehicle, brv_serialize_callback callback);

// converts brick parameters to properties
void brv_serialze(brv_vehicle* vehicle, brv_deserialize_callback callback); 
#endif

// destroys vehicle
void brv_close(brv_vehicle vehicle);

// returns brv from brv_vehicle
void brv_build(brv_vehicle vehicle, unsigned int* size, unsigned char** data);


