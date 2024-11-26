#include "../include/brv.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "memory.h"

brv_vehicle brv_read(unsigned char* contents) {
  brv_vehicle vehicle = {};
  vehicle.version = contents[0];
  vehicle.numobjects = contents[1]+contents[2]*256;


  // check if version is supported
  if (vehicle.version>BR_SAVE_VERSION) {
    printf("BRV Version %i is unsupported\n", vehicle.version);
    return vehicle;
  }
  printf("Vehicle version: %i\n", vehicle.version);

  // brick rigs uses 2 different systems for loading vehicles
  if (vehicle.version<BR_SAVE_INTERFACE_VERSION) {
    printf("Vehicle uses legacy brv\n");
    goto legacy;
  } else {
    printf("Vehicle uses 1.0+ brv\n");
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

bricktraverse:
    int bricklen = 
      contents[p]+
      256*contents[p+1]+
      256*256*contents[p+2]+
      256*256*256*contents[p+3];

    // fixes phantom errors
    if (bricklen>255) {
      p++;
      goto bricktraverse;
    };

    p+=4;

    // handle brick class
    char* brick = (char*)malloc(bricklen);
    memcpy(brick,(void*)(contents+p), bricklen);
    // now we remove instance id in the end
    // BP_ScalableZylinder_C_3 -> BP_ScalableZylinder_C
    int currentchar = bricklen-1;
    while (1) {
      if (brick[currentchar]=='C') {
        break;
      }
      if (brick[currentchar]=='_') {
        brick[currentchar] = '\0';
        break;
      }
      brick[currentchar] = '\0';
      currentchar--;
    }
    
    

    p+=bricklen;
    printf("\n  %s\n",brick);

    // traverse through all properties
    char properties = contents[p++];
    printf("  with %i properties:\n",properties);
    for (int j = 0;j<properties;j++) {
      int propertylen = contents[p]+256*contents[p+1]+256*256*contents[p+2]+256*256*256*contents[p+3];
      p+=4;

      // get property name and data
      char* property = (char*)malloc(propertylen);
      memcpy(property,(void*)(contents+p), propertylen);
      
      p+=propertylen;
      char datasize = contents[p++];
      printf("    %s: %i bytes\n",property, datasize);
      p+=datasize;
    }

    // Fluppi393: Legacy version where the brick location accuracy was increased
    // kotofyt: not sure where it is used since v6 uses v3's
    if (vehicle.version!=BR_SAVE_SMALLER_STEPS_VERSION) {
      short x = contents[p+0]+256*contents[p+1];
      short y = contents[p+2]+256*contents[p+3];
      short z = contents[p+4]+256*contents[p+5];
      char rotx = contents[p+6];
      char roty = contents[p+7];
      char rotz = contents[p+8];
      p+=9;
      printf("  at: %i, %i, %i\n",x,y,z);
    } 
  }
  

  return vehicle;

remake:

  vehicle.numclasses = contents[3]+contents[4]*256;
  vehicle.numproperties = contents[5]+contents[6]*256;
  return vehicle;
};
void brv_close(brv_vehicle vehicle) {

};
