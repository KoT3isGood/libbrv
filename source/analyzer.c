#include "../include/brv.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "memory.h"


void brv_deserialize(brv_vehicle* vehicle, brv_serialize_callback callback) {
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

void brv_serialze(brv_vehicle* vehicle, brv_deserialize_callback callback) {
  for (brv_brick* brick=vehicle->bricks;brick;brick=brick->next) {
    // compute num of params
    int numparams=0;

    if (brick->operation) {
      numparams++;
    }

    if (brick->input.isactive) {
      numparams++;
      if (brick->input.mode==BRV_INPUT_VALUE) {
        numparams++;
      }
      if (brick->input.mode==BRV_INPUT_SOURCE_BRICKS) {
        numparams++;
      }
    }
    if (brick->inputa.isactive) {
      numparams++;
      if (brick->inputa.mode==BRV_INPUT_VALUE) {
        numparams++;
      }
      if (brick->inputa.mode==BRV_INPUT_SOURCE_BRICKS) {
        numparams++;
      }
    }
    if (brick->inputb.isactive) {
      numparams++;
      if (brick->inputb.mode==BRV_INPUT_VALUE) {
        numparams++;
      }
      if (brick->inputb.mode==BRV_INPUT_SOURCE_BRICKS) {
        numparams++;
      }
    }
    if(brick->issizeactive) {
      numparams++;
    }

    if (brick->output.isactive) {
      numparams+=4;  
    }
    // reset to zero
    numparams++;

    brick->numparameters=numparams;
    brick->parameters = (brv_brick_parameter*)malloc(sizeof(brv_brick_parameter)*numparams);

    int i=0;

    if (brick->issizeactive) {
      brick->parameters[i].name="BrickSize";
      brick->parameters[i].datasize=12;
      brick->parameters[i].data=malloc(12);
      memcpy(brick->parameters[i].data,brick->size,12);
      i++;
    }

    if (brick->output.isactive) {
      brick->parameters[i].name="OutputChannel.MaxIn";
      brick->parameters[i].datasize=4;
      brick->parameters[i].data=malloc(4);
      memcpy(brick->parameters[i].data,&brick->output.maxin,4);
      i++;
      brick->parameters[i].name="OutputChannel.MinIn";
      brick->parameters[i].datasize=4;
      brick->parameters[i].data=malloc(4);
      memcpy(brick->parameters[i].data,&brick->output.minin,4);
      i++;
      brick->parameters[i].name="OutputChannel.MaxOut";
      brick->parameters[i].datasize=4;
      brick->parameters[i].data=malloc(4);
      memcpy(brick->parameters[i].data,&brick->output.maxout,4);
      i++;
      brick->parameters[i].name="OutputChannel.MinOut";
      brick->parameters[i].datasize=4;
      brick->parameters[i].data=malloc(4);
      memcpy(brick->parameters[i].data,&brick->output.minout,4);
      i++;

    }

      brick->parameters[i].name="bReturnToZero";
      brick->parameters[i].datasize=1;
      brick->parameters[i].data=malloc(1);
      memcpy(brick->parameters[i].data,&brick->resettozero,1);
      i++;












    if (brick->operation) {

      brick->parameters[i].name="Operation";
      char* value;
      if (!strcmp(brick->operation,"+")) {
        value="Add";
      }     
      else if (!strcmp(brick->operation,"-")) {
        value="Subtract";
      }
      else if (!strcmp(brick->operation,"*")) {
        value="Multiply";
      }     
      else if (!strcmp(brick->operation,"/")) {
        value="Divide";
      }     
      else if (!strcmp(brick->operation,">")) {
        value="Greater";
      }     
      else if (!strcmp(brick->operation,"^")) {
        value="Power";
      }     
      else if (!strcmp(brick->operation,"%")) {
        value="Fmod";
      }     
      else if (!strcmp(brick->operation,"<")) {
        value="Less";
      }     
      else if (!strcmp(brick->operation,"floor")) {
        value="Floor";
      }     
      else if (!strcmp(brick->operation,"min")) {
        value="Min";
      }     
      else if (!strcmp(brick->operation,"max")) {
        value="Max";
      }     
      else if (!strcmp(brick->operation,"abs")) {
        value="Abs";
      }     
      else if (!strcmp(brick->operation,"sign")) {
        value="Sign";
      }     
      else if (!strcmp(brick->operation,"sqrt")) {
        value="Sqrt";
      }     
      else if (!strcmp(brick->operation,"ceil")) {
        value="Ceil";
      }     
      else if (!strcmp(brick->operation,"round")) {
        value="Round";
      }    
      else if (!strcmp(brick->operation,"sind")) {
        value="SinDeg";
      }     
      else if (!strcmp(brick->operation,"sin")) {
        value="Sin";
      }   
      else if (!strcmp(brick->operation,"cosd")) {
        value="CosDeg";
      }     
      else if (!strcmp(brick->operation,"cos")) {
        value="Cos";
      }
      else if (!strcmp(brick->operation,"tand")) {
        value="TanDeg";
      }     
      else if (!strcmp(brick->operation,"tan")) {
        value="Tan";
      }  
      else if (!strcmp(brick->operation,"asind")) {
        value="AsinDeg";
      }     
      else if (!strcmp(brick->operation,"asin")) {
        value="Asin";
      }   
      else if (!strcmp(brick->operation,"acosd")) {
        value="AcosDeg";
      }     
      else if (!strcmp(brick->operation,"acos")) {
        value="Acos";
      }
      else if (!strcmp(brick->operation,"atand")) {
        value="AtanDeg";
      }     
      else if (!strcmp(brick->operation,"atan")) {
        value="Atan";
      }              
      brick->parameters[i].datasize=strlen(value)+1;
      brick->parameters[i].data=malloc(strlen(value)+1);
      ((char*)brick->parameters[i].data)[0]=strlen(value);
      memcpy(brick->parameters[i].data+1,value,strlen(value));



      
      i++;
    }



    if (brick->input.isactive) {
      brick->parameters[i].name="InputChannel.InputAxis";

      if (brick->input.mode==BRV_INPUT_VALUE) {
        const char* value = "AlwaysOn";
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      } else
      if (brick->input.mode==BRV_INPUT_SOURCE_BRICKS) {
        const char* value = "Custom";
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      } else {
        const char* value = brick->inputa.inputaxis;
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));      
      }


      switch (brick->input.mode) {
        case BRV_INPUT_VALUE:
          i++;
          brick->parameters[i].name="InputChannel.Value";
          brick->parameters[i].datasize=4;
          brick->parameters[i].data=malloc(4);
          memcpy(brick->parameters[i].data,&brick->input.value,4);break;
        case BRV_INPUT_SOURCE_BRICKS:
          i++;
          brick->parameters[i].name="InputChannel.SourceBricks";
          brick->parameters[i].datasize=2+2*brick->input.numsourcebricks;
          brick->parameters[i].data=malloc(brick->parameters[i].datasize);

          memcpy(brick->parameters[i].data,&brick->input.numsourcebricks,2);
          for (int j=0;j<brick->input.numsourcebricks;j++) {
            int foundid=1;
            char found=0;
            for (brv_brick* brick2=vehicle->bricks;brick2;brick2=brick2->next) {
              if (brick2==brick->input.sourcebricks[j]) {
                found=1;
                break;
              }
              foundid++;
            }
            if (found==0) {
              printf("failed to find brick %p",&brick->input.sourcebricks[j]);
            }
            memcpy(brick->parameters[i].data+2+2*j,&foundid,2);
          }
          break;

      }
      i++;
    }
    if (brick->inputa.isactive) {
      brick->parameters[i].name="InputChannelA.InputAxis";

      if (brick->inputa.mode==BRV_INPUT_VALUE) {
        const char* value = "AlwaysOn";
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      } else
      if (brick->inputa.mode==BRV_INPUT_SOURCE_BRICKS) {
        const char* value = "Custom";
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      } else {
        const char* value = brick->inputa.inputaxis;
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      }


      switch (brick->inputa.mode) {
        case BRV_INPUT_VALUE:
          i++;
          brick->parameters[i].name="InputChannelA.Value";
          brick->parameters[i].datasize=4;
          brick->parameters[i].data=malloc(4);
          memcpy(brick->parameters[i].data,&brick->inputa.value,4);break;
        case BRV_INPUT_SOURCE_BRICKS:
          i++;
          brick->parameters[i].name="InputChannelA.SourceBricks";
          brick->parameters[i].datasize=2+2*brick->inputa.numsourcebricks;
          brick->parameters[i].data=malloc(brick->parameters[i].datasize);

          memcpy(brick->parameters[i].data,&brick->inputa.numsourcebricks,2);
          for (int j=0;j<brick->inputa.numsourcebricks;j++) {
            int foundid=1;
            char found=0;
            for (brv_brick* brick2=vehicle->bricks;brick2;brick2=brick2->next) {
              if (brick2==brick->inputa.sourcebricks[j]) {
                found=1;
                break;
              }
              foundid++;
            }
            if (found==0) {
              printf("failed to find brick %p",&brick->inputa.sourcebricks[j]);
            }
            memcpy(brick->parameters[i].data+2+2*j,&foundid,2);
          }
          break;

      }
      i++;
      
    }
    if (brick->inputb.isactive) {
      brick->parameters[i].name="InputChannelB.InputAxis";

      if (brick->inputb.mode==BRV_INPUT_VALUE) {
        const char* value = "AlwaysOn";
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      } else
      if (brick->inputb.mode==BRV_INPUT_SOURCE_BRICKS) {
        const char* value = "Custom";
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));
      } else {
        const char* value = brick->inputa.inputaxis;
        brick->parameters[i].datasize=strlen(value)+1;
        brick->parameters[i].data=malloc(strlen(value)+1);
        ((char*)brick->parameters[i].data)[0]=strlen(value);
        memcpy(brick->parameters[i].data+1,value,strlen(value));    
      }


      switch (brick->inputb.mode) {
        case BRV_INPUT_VALUE:
          i++;
          brick->parameters[i].name="InputChannelB.Value";
          brick->parameters[i].datasize=4;
          brick->parameters[i].data=malloc(4);
          memcpy(brick->parameters[i].data,&brick->inputb.value,4);
          break;
        case BRV_INPUT_SOURCE_BRICKS:
          i++;
          brick->parameters[i].name="InputChannelB.SourceBricks";
          brick->parameters[i].datasize=2+2*brick->inputb.numsourcebricks;
          brick->parameters[i].data=malloc(brick->parameters[i].datasize);

          memcpy(brick->parameters[i].data,&brick->inputb.numsourcebricks,2);
          for (int j=0;j<brick->inputb.numsourcebricks;j++) {
            int foundid=1;
            char found=0;
            for (brv_brick* brick2=vehicle->bricks;brick2;brick2=brick2->next) {
              if (brick2==brick->inputb.sourcebricks[j]) {
                found=1;
                break;
              }
              foundid++;
            }
            if (found==0) {
              printf("failed to find brick %p",&brick->inputb.sourcebricks[j]);
            }
            memcpy(brick->parameters[i].data+2+2*j,&foundid,2);
          }
          break;

      }
      i++;

    }

  }
}; 


