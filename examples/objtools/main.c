
#include "../../include/brv.h"
#include "getopt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"


int main(int argc, char** argv) {
  int opt;
  char *inputfile = 0;
  char *outputfile = 0;
  char *mtloutputfile = 0;
  char isobj;
  if (argc<2) {
    printf("error: no arguments provided\n");
    exit(1);
  }

  // Define long options
  static struct option long_options[] = {
      {"help",    no_argument,       0, 'h'},
      {"brv",   required_argument, 0, 1},
      {"obj",   required_argument, 0, 2},
      {"output",   required_argument, 0, 'o'},
      {"mtl",   required_argument, 0, 3},
      {0, 0, 0, 0}
  };

  // Parse options
  while ((opt = getopt_long(argc, argv, "ho:", long_options, 0)) != -1) {
    printf("%s\n",long_options[optind - 1].name);
      switch (opt) {
          case 1:
            if (inputfile) {
              continue;
            }
            isobj=0;
            inputfile = optarg;
            break;
          case 2:
            if (inputfile) {
              continue;
            }
            isobj=0;
            inputfile = optarg;
            break;
          case 3:
            if (mtloutputfile) {
              continue;
            }
            isobj=0;
            mtloutputfile = optarg;
            break;
          case 'h':
            printf(".obj to .brv format converter\n");
            printf("-h, --help                                    shows this message\n");
            printf("--brv                                         input for brv to be converted to obj\n");
            printf("--obj                                         input for obj to be converted to brv\n");
            printf("-o, --output (default: a.obj or Vehicle.brv)  output for file\n");
            printf("--mtl                                         file for material\n");
            exit(0);
          case 'o':
              outputfile = optarg;
              break;
          case '?':
              //printf("undefined\n");
              break;
          default:
              //printf("default\n");
              break;
      }
  }

  if (!inputfile) {
    printf("error: missing argument\n");
    exit(1);
  }

  // read input file
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

  // handle brvs
  if (!isobj) {
    brv_vehicle vehicle = brv_read(filedata);
    // read brv
    brv_deserialize(&vehicle,0);
    
    for (brv_brick* brick=vehicle.bricks;brick;brick=brick->next) {
      // find mesh by it's name
    }

    brv_close(vehicle);
  }
}


struct objdata {
	uint32_t vertices;
	uint32_t indicies;
  void* vertexBuffer;
  void* indexBuffer;
} readobj(char* file) {

  struct objdata brick = {};
  FILE* f = fopen(file,"r");
	uint32_t size = 0;
  fseek(f, 0, SEEK_END); // seek to end of file
  size = ftell(f); // get current file pointer
  fseek(f, 0, SEEK_SET); // seek back to beginning of file
  char* fileData = (char*)malloc(size+1);
  fread(fileData, sizeof(char), size, f); 
  if (size==0) return brick;
	uint32_t vertices = 0;
	uint32_t uvs = 0;
	uint32_t normals = 0;
	uint32_t indicies = 0;
	for (uint32_t i = 0; i < size; i++) {
		if ((i + 1) > size) break; // file ending

		if (fileData[i] == '\n' && fileData[i+1] == 'v'&& fileData[i + 2] == ' ') {
			vertices++;
		};
		if (fileData[i] == '\n' && fileData[i + 1] == 'v' && fileData[i + 2] == 't' && fileData[i + 3] == ' ') {
			uvs++;
		};
		if (fileData[i] == '\n' && fileData[i + 1] == 'f' && fileData[i + 2] == ' ') {
			indicies++;
		};
	}
	printf("Vertices: %i\n", vertices);
	printf("UVs: %i\n", uvs);
	printf("Indexes: %i\n", indicies);

	uint32_t startPointer = 0;
	uint32_t endPointer = 0;

	void* vertexBufferAlloc = malloc(vertices*12);
	void* uvBufferAlloc = malloc(uvs*8);
	void* indexBufferAlloc = malloc(indicies*12);
	void* uvIndexBufferAlloc = malloc(indicies*12);

	uint32_t amountOfIndicies = indicies * 3;
	uint32_t amountOfVertices = vertices * 3;

	vertices = 0;
	uvs = 0;
	indicies = 0;
	for (uint32_t i = 0; i < size; i++) {
		if ((i + 1) > size) break; // file ending

		uint32_t type = 0;
		uint32_t start = i+1;

		if (fileData[i] == '\n' && fileData[i + 1] == 'v' && fileData[i + 2] == ' ') {
			vertices++;
			type = 1;
		};
		if (fileData[i] == '\n' && fileData[i + 1] == 'f' && fileData[i + 2] == ' ') {
			indicies++;
			type = 2;
			
		};
		if (fileData[i] == '\n' && fileData[i + 1] == 'v' && fileData[i + 2] == 't' && fileData[i + 3] == ' ') {
			uvs++;
			type = 3;

		};
    if (type != 0) {
			i++;
			while (fileData[i] != '\n') {
				i++;
			}
			fileData[i] = '\0';
			if (type == 1) {
				float* v1 = (float*)((uint64_t)vertexBufferAlloc + 12 * (vertices - 1));
				float* v2 = (float*)((uint64_t)vertexBufferAlloc + 4 + 12 * (vertices - 1));
				float* v3 = (float*)((uint64_t)vertexBufferAlloc + 8 + 12 * (vertices - 1));
				sscanf((char*)&fileData[start], "v %f %f %f", v1, v2, v3);
			}
			if (type == 2) {
				uint32_t* v1 = (uint32_t*)((uint64_t)indexBufferAlloc + 12 * (indicies - 1));
				uint32_t* v2 = (uint32_t*)((uint64_t)indexBufferAlloc + 4 + 12 * (indicies - 1));
				uint32_t* v3 = (uint32_t*)((uint64_t)indexBufferAlloc + 8 + 12 * (indicies - 1));

				uint32_t* vt1 = (uint32_t*)((uint64_t)uvIndexBufferAlloc + 12 * (indicies - 1));
				uint32_t* vt2 = (uint32_t*)((uint64_t)uvIndexBufferAlloc + 4 + 12 * (indicies - 1));
				uint32_t* vt3 = (uint32_t*)((uint64_t)uvIndexBufferAlloc + 8 + 12 * (indicies - 1));
				uint32_t none = 0;
				sscanf((char*)&fileData[start], "f %i/%i/%i %i/%i/%i %i/%i/%i", v1, vt1, &none, v2, vt2, &none, v3, vt3, &none);
				*v1 -= 1;
				*v2 -= 1;
				*v3 -= 1;
				*vt1 -= 1;
				*vt2 -= 1;
				*vt3 -= 1;
			}
			if (type == 3) {
				float* v1 = (float*)((uint64_t)uvBufferAlloc + 8 * (uvs - 1));
				float* v2 = (float*)((uint64_t)uvBufferAlloc + 4 + 8 * (uvs - 1));
				sscanf((char*)&fileData[start], "vt %f %f", v1, v2);
			}
			fileData[i] = '\n';
			i--;
		}
	};
  brick.indicies=amountOfIndicies;
  brick.vertices=amountOfVertices;
  brick.vertexBuffer=vertexBufferAlloc;
  brick.indexBuffer=indexBufferAlloc;
  return brick;
}
