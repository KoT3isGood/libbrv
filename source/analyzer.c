#include "../include/brv.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "memory.h"


void brv_analyze(brv_vehicle* vehicle, brv_analyze_callback callback) {
  if (vehicle->version<BR_SAVE_INTERFACE_VERSION) {
    goto legacy;
  } else {
    goto remake;
  }

legacy:
  ;
  brv_brick* brick = 0;
  for (brick=vehicle->bricks;brick;brick=brick->next) {
    for (int i = 0;i<brick->numparameters;i++) {
      char* name = brick->parameters[i].name;
      unsigned char* contents = brick->parameters[i].data;
      if (!strcmp(name,"BrickMaterial")) {
        int size = 
          contents[0]+
          256*contents[1]+
          256*256*contents[2]+
          256*256*256*contents[3];
        brick->material.material=contents+4;
        //printf("material: %s\n", brick->material.material);
        continue;
      }
      if (!strcmp(name,"BrickPaint")) {
       int size = 
          contents[0]+
          256*contents[1]+
          256*256*contents[2]+
          256*256*256*contents[3];
        brick->material.material=contents+4;
        //printf("paint: %s\n", brick->material.material);
        continue;

      }
      if (!strcmp(name,"BrickScale")) {
        brick->size[0]=contents[0];
        brick->size[1]=contents[1];
        brick->size[2]=contents[2];
        //printf("size: %f,%f,%f\n",brick->size[0],brick->size[1],brick->size[2]);
        continue;
      }
      // TODO: implement the rest of the stuff
    }

  }
  return;
remake:
  brick = 0;
  for (brick=vehicle->bricks;brick;brick=brick->next) {
    for (int i = 0;i<brick->numparameters;i++) {
      char* name = brick->parameters[i].name;
      unsigned char* contents = brick->parameters[i].data;
      if (!strcmp(name,"BrickSize")) {
        brick->size[0]=contents[0];
        brick->size[1]=contents[1];
        brick->size[2]=contents[2];
        continue;
      }
      if (!strcmp(name,"Text")) {
        printf("%i\n",brick->parameters[i].datasize-2);
        for (int j =0;j<brick->parameters[i].datasize;j++) {
          printf("%c",((char*)brick->parameters[i].data)[j]);
        }
        printf("\n");
      }

    }
  }

  return;
};
