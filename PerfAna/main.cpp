#include <stdio.h>
#include <chrono>
#include <thread>

#include "test.h"

BENCHMARK_MAIN();

// int main(int argc, char** argv) {                                     
//     char arg0_default[] = "benchmark";                                  
//     char* args_default = arg0_default;                                  
//     if (!argv) {                                                        
//         argc = 1;                                                         
//         argv = &args_default;                                             
//     }                                                                   
//     benchmark::Initialize(&argc, argv);
// }
