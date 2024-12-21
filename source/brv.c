#include "../include/brv.h"
#include "stdlib.h"
#include "stdio.h"

char** dsp;
unsigned int numdsp;

void libbrv_init(const char** dynamically_sized_properties, unsigned int num) {
  dsp=(char*)dynamically_sized_properties;
  numdsp=num;
};
