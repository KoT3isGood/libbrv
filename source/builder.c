#include "../include/brv.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include <math.h>

// for 1.0+
typedef struct brv_class {
  struct brv_class* next;
  int classid;
  char* name;
} brv_class;

typedef struct brv_brick_parameter2 {
  unsigned int numelements;
  unsigned int datasize;
  unsigned int size;
  char* name;
  bool isparamsizeconstant;
  int propertyid;


  struct parameter {
    struct parameter* next;
    int paramid;
    brv_brick_parameter* param;
  }* parameters;
  struct parameter* startingparameters;
  struct brv_brick_parameter2* next;
} brv_brick_parameter2;

typedef struct brv_brick2 {
  int classid;
  int numparameters;
  struct brick_parameter {
    short propertyid;
    short paramid;
  }* parameters;
  float rotation[3];
  float location[3];
  struct brv_brick2* next;
} brv_brick2;


extern char** dsp;
extern unsigned int numdsp;


void brv_build(brv_vehicle vehicle, unsigned int* size, unsigned char** data) {
  if (!dsp) {
    printf("failed to find dynamically sized properties\n");
  }
  if (!numdsp) {
    printf("failed to find dynamically sized properties\n");
  }


  brv_class* startingclass=0;
  brv_class* currentclass=0;
  printf("%p\n",currentclass);
  int numclasses=0;
  brv_brick_parameter2* startingparameters=0;
  brv_brick_parameter2* currentparameters=0;
  brv_brick2* startingbrick=0;
  brv_brick2* currentbrick=0;
  unsigned int filesize = 3;
  if (!data) {
    // uhh, it should be there
    printf("data is 0\n");
    return;
  }
  if (vehicle.version>=BR_SAVE_INTERFACE_VERSION) {
    int numparams=0;
    filesize+=4;
    for (brv_brick* brick=vehicle.bricks;brick;brick=brick->next) {
      brv_brick2* genbrick=(brv_brick2*)malloc(sizeof(brv_brick2));
      int foundid=0;
      bool found = 0;
      // create non-exisiting classes
      for(brv_class* class = startingclass;class;class=class->next) {
        if (!strcmp(class->name,brick->name)) {
          found=1;
          break;
        }
        foundid++;
      }

      if (found) {
      } else {
        brv_class* class = (brv_class*)malloc(sizeof(brv_class));
        class->name = (char*)malloc(strlen(brick->name)+1);
        class->next = 0;
        strcpy(class->name,brick->name);
        strcpy(class->name,class->name);
        if (currentclass) {
          currentclass->next = class;
          currentclass=currentclass->next;
        } else {
          startingclass = class;
          currentclass=class;
        }


        filesize+=strlen(brick->name)+1;
        numclasses++;
        //printf("after adding %s: %i\n",brick->name,filesize);
      }
      genbrick->classid=foundid;
      genbrick->parameters = (struct brick_parameter*)malloc(4*brick->numparameters);
      genbrick->numparameters = brick->numparameters;

      // same to properties
      // but we also have to check the data
      
      // too bad, but ehh,could be worse
      struct brick_parameter* startingparameter=0;
      struct brick_parameter* currentparameter=0;
      for (int i = 0;i<brick->numparameters;i++) {
        foundid = 0;
        found = 0;
        brv_brick_parameter2* param;
        for(brv_brick_parameter2* parameter = startingparameters;parameter;parameter=parameter->next) {
           if (!strcmp(parameter->name,brick->parameters[i].name)) {
            found=1;
            param=parameter;
            break;
          }
        foundid++;

        }
        if (found) {
        } else {
          param = (brv_brick_parameter2*)malloc(sizeof(brv_brick_parameter2));
          param->name = (char*)malloc(strlen(brick->parameters[i].name)+1);
          strcpy(param->name,brick->parameters[i].name);

          param->datasize=0;
          param->numelements=0;
          param->isparamsizeconstant=true;
          for (int i = 0;i<numdsp;i++) {
            if (!strcmp(dsp[i],param->name)) {
              param->isparamsizeconstant=false;
              break;
            }
          }

          if (currentparameters) {
            currentparameters->next = param;
            currentparameters=currentparameters->next;
          } else {
            startingparameters = param;
            currentparameters=param;
          }
          printf("huh?\n");

          numparams++;


          // name size + 6
          filesize+=strlen(brick->parameters[i].name)+7;
        }

        genbrick->parameters[i].propertyid=foundid;

        foundid = 0;
        found = 0; 
        // now find parameters
        for(struct parameter* par = param->startingparameters;par;par=par->next) {
          if (brick->parameters[i].datasize!=par->param->datasize) {
            foundid++;
            continue;
          }
          if (!memcmp(par->param->data,brick->parameters[i].data,par->param->datasize)) {
            found = 1;
            break;
          }
          foundid++;
        }
        
        genbrick->parameters[i].paramid=foundid;
        if (found) {
          //printf("found existing param data: %s\n",brick->parameters[i].name);
        } else {
          struct parameter* startingparam=param->startingparameters;
          struct parameter* currentparam=param->parameters;


          struct parameter* par = (struct parameter*)malloc(sizeof(struct parameter));
          par->next=0;
          if (currentparam) {
            currentparam->next = par;
            currentparam=currentparam->next;
          } else {
            startingparam = par;
            currentparam=par;
          }


          par->param=&brick->parameters[i];
          param->datasize+=par->param->datasize;

          printf("ishallah we need new param data for %s: %i\n",brick->parameters[i].name,brick->parameters[i].datasize);
          
       
          //printf("_ %i\n",filesize);
          filesize+=brick->parameters[i].datasize;
          param->numelements++;
          param->startingparameters = startingparam;
          param->parameters=currentparam;
          if (!param->isparamsizeconstant) {
            filesize+=2;
          }
        }
      }
      printf("-------------------------------\n");


      brick->position[0]*=100;
      brick->position[1]*=100;
      brick->position[2]*=100;
      memcpy(genbrick->location,brick->position,12);
      memcpy(genbrick->rotation,brick->rotation,12);
      if (currentbrick) {
        currentbrick->next = genbrick;
        currentbrick=currentbrick->next;
      } else {
        startingbrick = genbrick;
        currentbrick=genbrick;
      }
      filesize+=31;
      filesize+=4*genbrick->numparameters;

    }
    printf("before %i\n",filesize);
    for(brv_brick_parameter2* parameter = startingparameters;parameter;parameter=parameter->next) {
      if (!parameter->isparamsizeconstant) {
        int size = 0;
        bool canbeshorted=true;
        for (struct parameter* param=parameter->startingparameters;param;param=param->next) {
          if (size==0) {
            size=param->param->datasize;
            continue;
          }
          if (size!=param->param->datasize) {
            canbeshorted=false;
          };
        }
        printf("canbeshorted %s %i\n",parameter->name,canbeshorted);
        if (canbeshorted) {
          filesize-=2*parameter->numelements-2;
          parameter->size=size;
        }
      }
    };
    printf("after %i\n",filesize);
    // fixes vehicle corruption
    filesize=ceil(filesize/16.0f)*16;
    
    *data = (unsigned char*)malloc(filesize);
    
    // data generation
    unsigned char* d=*data;
    unsigned int p = 0;
    d[p++]=vehicle.version;
    memcpy(&d[p],&vehicle.numobjects,2);
    p+=2;
    memcpy(&d[p],&vehicle.numclasses,2);
    p+=2;
    memcpy(&d[p],&vehicle.numproperties,2);
    p+=2;
    for (brv_class* brick=startingclass;brick;brick=brick->next) {
      d[p++]=(char)strlen(brick->name);
      memcpy(&d[p],brick->name,(char)strlen(brick->name));
      p+=strlen(brick->name);
    }

    for (brv_brick_parameter2* parameter=startingparameters;parameter;parameter=parameter->next) {
      d[p++]=(char)strlen(parameter->name);
      memcpy(&d[p],parameter->name,(char)strlen(parameter->name));
      p+=strlen(parameter->name);
      memcpy(&d[p],&parameter->numelements,2);
      p+=2;
      memcpy(&d[p],&parameter->datasize,4);
      p+=4;

      // now data
      for (struct parameter* param=parameter->startingparameters;param;param=param->next) {
        memcpy(&d[p],param->param->data,param->param->datasize);
        p+=param->param->datasize;
      }
      // sizes last
      if (!parameter->isparamsizeconstant) {

        if (parameter->numelements<2) {

        } else {
        if (parameter->size) {
          memcpy(&d[p],&parameter->size,2);
          p+=2;
        } else { 
            memset(&d[p],0,2);
            p+=2;
            int datasize=2;
            for (struct parameter* param=parameter->startingparameters;param;param=param->next) {
              memcpy(&d[p],&param->param->datasize,2);
              p+=2;
            }
          }
        }
      }
    }

    // now bricks

    for (brv_brick2* brick=startingbrick;brick;brick=brick->next) {
      memcpy(&d[p], &brick->classid,2);
      p+=2;
      unsigned int memory = 25;
      memory+=brick->numparameters*4;

      memcpy(&d[p], &memory,4);
      p+=4;
      memcpy(&d[p++], &brick->numparameters,1);

      for (int i = 0;i<brick->numparameters;i++) {
        memcpy(&d[p], &brick->parameters[i].propertyid,2);
        p+=2;
        memcpy(&d[p], &brick->parameters[i].paramid,2);
        p+=2;
      }
      memcpy(&d[p], &brick->location,12);
      p+=12;
      memcpy(&d[p], &brick->rotation,12);
      p+=12;
    }
    printf("%i\n",p);
  }
  *size = filesize;

  printf("%i\n",filesize);

  return;
};
