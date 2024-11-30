#include "stdio.h"
#include "stdlib.h"
#include "../include/brv.h"



int main() {

  FILE* file = fopen("Vehicle.brv","r");
  fseek(file,0,SEEK_END);
  unsigned int filesize=ftell(file);
  printf("size:%i\n",filesize);
  fseek(file,0,SEEK_SET);
  unsigned char* filedata=malloc(filesize);
  fread(filedata,1,filesize,file);
  fclose(file);

  brv_vehicle vehicle = brv_read(filedata);
  brv_analyze(&vehicle,0);
  printf("%s\n",vehicle.bricks->position);
  brv_close(vehicle);
}
