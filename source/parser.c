#include "../include/brv.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "memory.h"




brv_vehicle brv_read(unsigned char* contents) {
  brv_vehicle vehicle = {};
  vehicle.version = contents[0];
  printf("version:%i\n",vehicle.version);
  vehicle.numobjects = contents[1]+contents[2]*256;

  brv_brick* startingbrick = 0;
  brv_brick* currentbrick = 0;

  // check if version is supported
  if (vehicle.version>BR_SAVE_VERSION) {
    printf("BRV Version %i is unsupported\n", vehicle.version);
    return vehicle;
  }

  // brick rigs uses 2 different systems for loading vehicles
  if (vehicle.version<BR_SAVE_INTERFACE_VERSION) {
    goto legacy;
  } else {
    goto remake;
  }

  int p = 0;

legacy:
  p=3;
  printf("Found %i objects:\n",vehicle.numobjects);
  for (int i = 0;i<vehicle.numobjects;i++) {
    brv_brick* vbrick = (brv_brick*)malloc(sizeof(brv_brick));

    vbrick->next = vehicle.bricks;
    vehicle.bricks = vbrick;

bricktraverse_legacy:
    ; 
    int bricklen = 
      contents[p]+
      256*contents[p+1]+
      256*256*contents[p+2]+
      256*256*256*contents[p+3];

    // fixes phantom errors
    if (bricklen>255) {
      p++;
      goto bricktraverse_legacy;
    };

    p+=4;

    // handle brick class
    char* brickname = (char*)malloc(bricklen);
    memcpy(brickname,(void*)(contents+p), bricklen);
    // now we remove instance id in the end
    // BP_ScalableZylinder_C_3 -> ScalableZylinder
    // we don't care about the size, im too lazy to implement this
    int currentchar = bricklen-1;

    // delete index 
    while (1) {
      if (brickname[currentchar]=='C') {
        break;
      }
      if (brickname[currentchar]=='_') {
        brickname[currentchar] = '\0';
        break;
      }
      brickname[currentchar] = '\0';
      currentchar--;
    }


    

    p+=bricklen;

    brv_brick* brick = (brv_brick*)malloc(sizeof(brv_brick));
    brick->name = brickname;

    // traverse through all properties
    char properties = contents[p++];
    brick->numparameters = properties;
    brick->parameters = (brv_brick_parameter*)malloc(sizeof(brv_brick_parameter)*properties);
    for (int j = 0;j<properties;j++) {
      int propertylen = contents[p]+256*contents[p+1]+256*256*contents[p+2]+256*256*256*contents[p+3];
      p+=4;
    

      char* property = (char*)malloc(propertylen);
      memcpy(property,(void*)(contents+p), propertylen);

      brv_brick_parameter parameter;
      parameter.name = property;

      
      p+=propertylen;
      char datasize = contents[p++];
      //printf("    %s: %i bytes\n",property, datasize);

      parameter.datasize = datasize;

      parameter.data = malloc(datasize);
      memcpy(parameter.data,contents+p,datasize);

      brick->parameters[j]=parameter;
      p+=datasize;
    }

    // Fluppi393: Legacy version where the brick location accuracy was increased
    // kotofyt: not sure where it is used since v6 uses v3's
    if (vehicle.version!=BR_SAVE_SMALLER_STEPS_VERSION) {
      short x = contents[p+0]+256*contents[p+1];
      brick->position[0] = x/100;
      short y = contents[p+2]+256*contents[p+3];
      brick->position[1] = y/100;
      short z = contents[p+4]+256*contents[p+5];
      brick->position[2] = z/100;

      char rotx = contents[p+6];
      brick->rotation[0]=rotx;
      char roty = contents[p+7];
      brick->rotation[1]=roty;
      char rotz = contents[p+8];
      brick->rotation[2]=rotz;

      p+=9;
    }

    brick->size[0]=1;
    brick->size[1]=1;
    brick->size[2]=1;
    // push into array
    if (currentbrick) {
      currentbrick->next = brick;
      currentbrick=currentbrick->next;
    } else {
      startingbrick = brick;
      currentbrick=brick;
    }
  }
  vehicle.bricks = startingbrick;  

  return vehicle;

remake:

  vehicle.numclasses = contents[3]+contents[4]*256;
  vehicle.numproperties = contents[5]+contents[6]*256;
  p=7;
  
  for (int i = 0;i<vehicle.numclasses;i++) {
    char classsize = contents[p++];
  }

  return vehicle;
};
void brv_close(brv_vehicle vehicle) {

};