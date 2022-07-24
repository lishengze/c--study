#pragma once

#include "global_declare.h"

#define BUF_SIZE 10
#define SHM_KEY 0x1234
 
struct shmseg {
   int cnt;
   int complete;
   char buf[BUF_SIZE];
};
