#pragma once

// from Source/BrickRigs/BrickEditor/BrickEditorSaveVersion.h
// Current save version, the legacy UBrickStatics version ended at 6
#define BR_SAVE_VERSION 14
// Version where brick units started to be saved as a float instead of uint16
#define BR_SAVE_BRICK_UNITS_FLOAT_VERSION 14
// Version where wheel meshes were moved so their connectors would line up with the origin
#define BR_SAVE_ALIGNED_WHEEL_ORIGIN_VERSION 14
// Prior to this version GetNameStringByIndex was used instead of GetNameStringByIndex in FEnumBrickPropertyBase::SerializeProperty
#define BR_SAVE_FIXED_ENUM_SERIALIZATION_VERSION 13
// Version where input channel indices where replaced by brick references
#define BR_SAVE_INPUT_CHANNEL_REFERENCE_VERSION 13
// Version where item mass and price started to be saved to the meta data
#define BR_SAVE_MASS_AND_PRICE 12
// Version where min and max sensor input values were replaced by universal input channel parameters
#define BR_SAVE_INPUT_CHANNEL_CURVE_VERSION 11
// Version where the min and max actuation limit properties have been inverted
#define BR_SAVE_ACTUATOR_LIMITS_VERSION 10
// Version where suspension damping was made player controllable
#define BR_SAVE_SUSPENSION_DAMPING_VERSION 9
// Version where the tags were added to the meta data
#define BR_SAVE_TAGS_VERSION 8
// Version where we transition from the UBrickStatics save functions to the UBrickEditorSaveInterface
#define BR_SAVE_INTERFACE_VERSION 7
// Version before the save interface was implemented
#define BR_SAVE_LAST_LEGACY_VERSION 6
// Version where input channels were introduced and the input axes enum has been changed
#define BR_SAVE_INPUT_CHANNEL_VERSION 6

// Legacy version where the brick location accuracy was increased
// kotofyt: not sure where it is used since v6 uses v3's
#define BR_SAVE_SMALLER_STEPS_VERSION 4
// Version where the saved element size has been fixed
#define BR_SAVE_FIXED_ELEMENT_SIZE_VERSION 3


// raw brick parameter
typedef struct {
  char* name;
  int datasize;
  void* data;
} brv_brick_parameter;

typedef struct brv_material {
  float color[3];
  float alpha;
  char* pattern;
  char* material;
} material;

typedef struct brv_brick {
  struct brv_brick* next;
  char* name;

  char numparameters;
  brv_brick_parameter* parameters;

  // materials
  struct brv_material material;
  // scalable bricks
  float size[3];

  float position[3];
  float rotation[3];
} brv_brick;



typedef struct {
  char version;
  short numobjects; 
  short numclasses; 
  short numproperties;

  // intermediate information from BR_SAVE_INTERFACE_VERSION+ 
  char** classes;
  brv_brick_parameter* parameters;

  brv_brick* bricks;

} brv_vehicle;

typedef void(*brv_analyze_callback)(const char*, int, void*);

// reads raw brv and gets all bricks and properties
brv_vehicle brv_read(unsigned char* contents);

// converts properties into brick parameters
void brv_analyze(brv_vehicle* vehicle, brv_analyze_callback callback); 

// destroys vehicle
void brv_close(brv_vehicle vehicle);
