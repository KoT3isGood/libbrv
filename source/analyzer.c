#include "../include/brv.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"
#include "math.h"

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
        continue;
      }
      // TODO: implement the rest of the stuff
    }

  }
  return;
remake:
  brick = 0;
  for (brick=vehicle->bricks;brick;brick=brick->next) {
    brick->type = 0;
    for (int i = 0;i<brick->numparameters;i++) {
      char* name = brick->parameters[i].name;
      unsigned char* contents = brick->parameters[i].data;

   
      if (!strcmp(name,"BrickSize")) {
        brick->type|=BRV_TYPE_SCALABLE_BRICK;
        if (vehicle->version<BR_SAVE_BRICK_UNITS_FLOAT_VERSION) {
        brick->size[0]=contents[0];
        brick->size[1]=contents[1];
        brick->size[2]=contents[2];
        } else {
          unsigned int x1 = (contents[0]<<0)+(contents[1]<<8)+((unsigned int)contents[2]<<16)+((unsigned int)contents[3]<<24);
          float x = *(float*)&x1/100.0;
          unsigned int y1 = (contents[4]<<0)+(contents[5]<<8)+((unsigned int)contents[6]<<16)+((unsigned int)contents[7]<<24);
          float y = *(float*)&y1/100.0;
          unsigned int z1 = (contents[8]<<0)+(contents[9]<<8)+((unsigned int)contents[10]<<16)+((unsigned int)contents[11]<<24);
          float z = *(float*)&z1/100.0;
          
          brick->size[0]=x*33.33333333333;
          brick->size[1]=y*33.33333333333;
          brick->size[2]=z*33.33333333333;

        }
        continue;
      }
      if (!strcmp(name, "BrickColor")) 	{
        // https://github.com/Inseckto/HSV-to-RGB/blob/master/HSV2RGB.c
        float r, g, b;
      
        float h = contents[0] / 255.0;
        float s = contents[1] / 255.0;
        float v = contents[2] / 255.0;
        
        int i = floor(h * 6);
        float f = h * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);
        
        switch (i % 6) {
          case 0: r = v, g = t, b = p; break;
          case 1: r = q, g = v, b = p; break;
          case 2: r = p, g = v, b = t; break;
          case 3: r = p, g = q, b = v; break;
          case 4: r = t, g = p, b = v; break;
          case 5: r = v, g = p, b = q; break;
        }


        brick->material.color[0]=r;
        brick->material.color[1]=g;
        brick->material.color[2]=b;
        brick->material.alpha=contents[3]/255.0;
      }
      if (!strcmp(name,"Text")) {
        brick->type|=BRV_TYPE_TEXT;
        char* text = (char*)malloc(contents[0]);
        memcpy(text,&contents[2],contents[0]);
        brick->text=text;
      }

      if (!strcmp(name,"WheelDiameter")) {
        brick->type|=BRV_TYPE_WHEEL;
        unsigned int x1 = (contents[0]<<0)+(contents[1]<<8)+((unsigned int)contents[2]<<16)+((unsigned int)contents[3]<<24);
        float x = *(float*)&x1/10.0;
        brick->wheel.diameter=x;
      }

      if (!strcmp(name,"WheelWidth")) {
        brick->type|=BRV_TYPE_WHEEL;
        unsigned int x1 = (contents[0]<<0)+(contents[1]<<8)+((unsigned int)contents[2]<<16)+((unsigned int)contents[3]<<24);
        float x = *(float*)&x1/10.0;
        brick->wheel.width=x;
      }



      if (!strcmp(name, "InputChannel")) {
        brick->type|=BRV_TYPE_INPUT;
        int a = 0;
        int numinputs=contents[0]+contents[1];
        brick->input.numsourcebricks=numinputs;
        brick->input.sourcebricks=(brv_brick**)malloc(sizeof(brv_brick*)*numinputs);
        for (int j = 0;j<numinputs;j++) {
          int brickid=contents[2+j]+contents[3+j*2];
          for (brv_brick* brick2=vehicle->bricks;brick2;brick2=brick2->next) {
            if (++a==brickid) {
              brick->input.sourcebricks[0]=brick2;
              break;
            };
          }
        }
      }
      if (!strcmp(name, "InputChannelA")) {      
        brick->type|=BRV_TYPE_MATH_BRICK;         
        int a = 0;                                        
        int numinputs=contents[0]+contents[1];
        brick->inputa.numsourcebricks=numinputs;
        brick->inputa.sourcebricks=(brv_brick**)malloc(sizeof(brv_brick*)*numinputs);
        for (int j = 0;j<numinputs;j++) {
          int brickid=contents[2+j]+contents[3+j*2];
          for (brv_brick* brick2=vehicle->bricks;brick2;brick2=brick2->next){
            if (++a==brickid) {   
              brick->inputa.sourcebricks[0]=brick2;
              break;                                         
            };   
          }
        }                              
      }          
      if (!strcmp(name, "InputChannelB")) {      
        brick->type|=BRV_TYPE_MATH_BRICK;         
        int a = 0;                                        
        int numinputs=contents[0]+contents[1];
        brick->inputb.numsourcebricks=numinputs;
        brick->inputb.sourcebricks=(brv_brick**)malloc(sizeof(brv_brick*)*numinputs);
        for (int j = 0;j<numinputs;j++) {
          int brickid=contents[2+j]+contents[3+j*2];
          for (brv_brick* brick2=vehicle->bricks;brick2;brick2=brick2->next){
            if (++a==brickid) {   
              brick->inputb.sourcebricks[0]=brick2;
              break;                                         
            };   
          }
        }                              
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

    if (brick->type&BRV_TYPE_INPUT) {
      numparams++;
      if (brick->input.mode!=BRV_INPUT_AXIS) {
        numparams++;
      }
      numparams+=5;  
    }

    if (brick->type&BRV_TYPE_MATH_BRICK) {
      numparams++;
      if (brick->inputa.mode==BRV_INPUT_VALUE) {
        numparams++;
      }
      if (brick->inputa.mode==BRV_INPUT_SOURCE_BRICKS) {
        numparams++;
      }
      numparams++;
      if (brick->inputb.mode==BRV_INPUT_VALUE) {
        numparams++;
      }
      if (brick->inputb.mode==BRV_INPUT_SOURCE_BRICKS) {
        numparams++;
      }
    }

    if(brick->type&BRV_TYPE_SCALABLE_BRICK) {
      numparams++;
    }

    brick->numparameters=numparams;
    brick->parameters = (brv_brick_parameter*)malloc(sizeof(brv_brick_parameter)*numparams);

    int i=0;

    // scalable bricks
    if (brick->type&BRV_TYPE_SCALABLE_BRICK) {
      brick->parameters[i].name="BrickSize";
      brick->parameters[i].datasize=12;
      brick->parameters[i].data=malloc(12);
      memcpy(brick->parameters[i].data,brick->size,12);
      i++;
    }

    //
    if (brick->type) {
      
    }


    if (brick->type&BRV_TYPE_INPUT) {
      // return to zero
      brick->parameters[i].name="bReturnToZero";
      brick->parameters[i].datasize=1;
      brick->parameters[i].data=malloc(1);
      memcpy(brick->parameters[i].data,&brick->resettozero,1);
      i++;

      // output channel
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


      // input axis
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




    if (brick->type&BRV_TYPE_MATH_BRICK) {
      // operator
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
      } else {
        value="Add";
      }        
      brick->parameters[i].datasize=strlen(value)+1;
      brick->parameters[i].data=malloc(strlen(value)+1);
      ((char*)brick->parameters[i].data)[0]=strlen(value);
      memcpy(brick->parameters[i].data+1,value,strlen(value)); 
      i++;
      
      // input channel a
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

    // input channel b
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
}; 


