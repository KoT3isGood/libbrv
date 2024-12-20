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
  struct brick_parameter {
    int propertyid;
    int paramid;
    struct brick_parameter* next;
  }* parameters;
  float rotation[3];
  float location[3];
  struct brv_brick2* next;
} brv_brick2;




void brv_build(brv_vehicle vehicle, unsigned int* size, unsigned char** data) {
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
      int foundid;
      bool found = 0;
      // create non-exisiting classes
      for(brv_class* class = startingclass;class;class=class->next) {
        printf("%s == %s\n",class->name,brick->name);
        if (!strcmp(class->name,brick->name)) {
          found=1;
          break;
        }
        foundid++;
      }

      if (found) {
        printf("found existing class: %s\n",brick->name);
      } else {
        printf("ishallah we need new class: %s\n",brick->name);
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
      genbrick->classid=numclasses-1;

      // same to properties
      // but we also have to check the data
      
      // too bad, but ehh,could be worse
      struct brick_parameter* startingparameter=0;
      struct brick_parameter* currentparameter=0;
      for (int i = 0;i<brick->numparameters;i++) {
      foundid = 0;
      found = 0;
        struct brick_parameter* genparam = (struct brick_parameter*)malloc(sizeof(struct brick_parameter));
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
          printf("found existing param: %s\n",brick->parameters[i].name);
        } else {
          param = (brv_brick_parameter2*)malloc(sizeof(brv_brick_parameter2));
          param->name = (char*)malloc(strlen(brick->parameters[i].name)+1);
          strcpy(param->name,brick->parameters[i].name);

          param->datasize=0;
          param->numelements=0;
          param->isparamsizeconstant=true;
          if (!strcmp(param->name,"Text")) {
            param->isparamsizeconstant=false;
          } else if(!strcmp(param->name,"BrickMaterial")) {
            param->isparamsizeconstant=false;
          }

          if (currentparameters) {
            currentparameters->next = param;
            currentparameters=currentparameters->next;
          } else {
            startingparameters = param;
            currentparameters=param;
          }

          numparams++;

          printf("ishallah we need new param: %s\n",brick->parameters[i].name);

          // name size + 6
          filesize+=strlen(brick->parameters[i].name)+9;
        }

        genparam->propertyid=numparams-1;

        foundid = 0;
        found = 0; 
        struct parameter* startingparam=param->parameters;
        struct parameter* currentparam=param->startingparameters;

        // now find parameters
        for(struct parameter* par = param->parameters;par;par=par->next) {
          if (brick->parameters[i].datasize!=par->param->datasize) {
            foundid++;
            continue;
          }
          if (!memcmp(par->param->data,brick->parameters[i].data,brick->parameters->datasize)) {
            found = 1;
            break;
          }
          foundid++;
        }
        if (found) {
          //printf("found existing param data: %s\n",brick->parameters[i].name);
        } else {
          struct parameter* par = (struct parameter*)malloc(sizeof(struct parameter));
          struct brick_parameter* parm = (struct brick_parameter*)malloc(sizeof(struct brick_parameter));
          par->next=0;
          if (currentparam) {
            currentparam->next = par;
            currentparam=currentparam->next;
          } else {
            startingparam = par;
            currentparam=par;
          }
          param->startingparameters = startingparam;
          param->parameters = currentparam;

          par->param=&brick->parameters[i];
          param->datasize+=par->param->datasize;

          printf("ishallah we need new param data for %s: %i\n",brick->parameters[i].name,brick->parameters[i].datasize);
       
          //printf("_ %i\n",filesize);
          filesize+=brick->parameters[i].datasize;
          param->numelements++;
          param->parameters=currentparam;
          //printf("%s:%i\n",param->name,param->datasize);
          if (!param->isparamsizeconstant) {
            filesize+=2;
          }
        }
      }
      printf("-------------------------------\n");
      if (currentbrick) {
        currentbrick->next = genbrick;
        currentbrick=currentbrick->next;
      } else {
        startingbrick = genbrick;
        currentbrick=genbrick;
      }

      }
      for(brv_brick_parameter2* parameter = startingparameters;parameter;parameter=parameter->next) {
      if (!parameter->isparamsizeconstant) {
        if (parameter->numelements<2) {
          filesize-=2;
        }
      }
    }
    
    // data generation
    *data = (unsigned char*)malloc(filesize);
    unsigned char* d=*data;
    unsigned int p = 0;
    d[p++]=vehicle.version;
    printf("%i\n",p);
    memcpy(&d[p],&vehicle.numobjects,2);
    p+=2;
    printf("%i\n",p);
    memcpy(&d[p],&vehicle.numclasses,2);
    p+=2;
    printf("%i\n",p);
    memcpy(&d[p],&vehicle.numproperties,2);
    p+=2;
    printf("%i\n",p);
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
      p+=2;
      if (parameter->isparamsizeconstant) {
        short size = (short)parameter->datasize/parameter->numelements;
        memcpy(&d[p-2],&size,2);
      } else {
        if (parameter->numelements<2) {
          memset(&d[p],0,2);
        } else {
          for (struct parameter* param=parameter->startingparameters;param;param=param->next) {
            memcpy(&d[p],&param->param->datasize,2);
            p+=2;
          }
        }
      }
    }


  }
  *size = filesize;

  printf("%i\n",filesize);

  return;
};
