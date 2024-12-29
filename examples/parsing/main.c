#include "../../include/brv.h"
#include "getopt.h"
#include "stdio.h"
#include "stdlib.h"


int main(int argc, char** argv) {
  int opt;
  char *inputfile = 0;
  if (argc<2) {
    printf("error: no arguments provided\n");
    exit(1);
  }

  // Define long options
  static struct option long_options[] = {
      {"help",    no_argument,       0, 'h'},
      {"input",   required_argument, 0, 'i'},
      {0, 0, 0, 0}
  };

  // Parse options
  while ((opt = getopt_long(argc, argv, "hi:o:", long_options, 0)) != -1) {
      switch (opt) {
          case 'h':
            printf("this application will print vehicle's bricks");
            printf("usage: -i <filename> or --input <filename>");
          case 'i':
              inputfile = optarg;
              break;
          case '?':
              printf("undefined\n");
              // getopt_long already printed an error message
          default:
              printf("default\n");
      }
  }
  if (!inputfile) {
    printf("error: missing argument");
    exit(1);
  }
  FILE* file = fopen(inputfile, "rb");
  if (!file) {
    printf("error: file not found");
    exit(1);
  }
  fseek(file,0,SEEK_END);
  unsigned int filesize=ftell(file);
  fseek(file,0,SEEK_SET);
  unsigned char* filedata=malloc(filesize);
  fread(filedata,1,filesize,file);
  fclose(file);

  printf("%s:\n",inputfile);
  brv_vehicle vehicle = brv_read(filedata);
  printf("version: %i\n",vehicle.version);
  printf("number of classes: %i\n",vehicle.numclasses);
  printf("number of properties: %i\n",vehicle.numproperties);
  printf("number of bricks: %i\n",vehicle.numobjects);
  printf("sizeof(%i)\n",sizeof(brv_brick));
  for (brv_brick* brick=vehicle.bricks;brick;brick=brick->next) {
    printf("  %s\n",brick->name);
    for (int i = 0;i<brick->numparameters;i++) {
      printf("  %i %s",brick->parameters[i].datasize,brick->parameters[i].name);
    }
    printf("\n");
  }

  brv_close(vehicle);

  return 0;
}
