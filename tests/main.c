#include "stdio.h"
#include "stdlib.h"
#include "../include/brv.h"



int main() {

  FILE* file = fopen("Vehicle.brv","rb");
  fseek(file,0,SEEK_END);
  unsigned int filesize=ftell(file);
  printf("size:%i\n",filesize);
  fseek(file,0,SEEK_SET);
  unsigned char* filedata=malloc(filesize);
  fread(filedata,1,filesize,file);
  fclose(file);

  brv_vehicle vehicle = brv_read(filedata);
  brv_analyze(&vehicle,0);
  printf("%f\n",vehicle.bricks->position[0]);
  unsigned int size = 0;
  unsigned char* reconstruction=0;
  brv_build(vehicle,&size,&reconstruction);
  file = fopen("Reconstruction.brv","wb");
  fwrite(reconstruction,1,size,file);
  fclose(file);
  brv_close(vehicle);
}
