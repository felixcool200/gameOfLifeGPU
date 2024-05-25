//Header guards
#ifndef KERNELS_H
#define KERNELS_H

#include <hip/hip_runtime.h>
#include "constants.h"

// Kernel BYTE Game of Life
__global__ void kernelOfLife(DATATYPE* lastFrame, DATATYPE* nextFrame, const int n, const int width, const int height);

// Kernel BIT Game of Life
__global__ void kernelOfLifeBIT(DATATYPE* lastFrame, DATATYPE* nextFrame, int n, int width, int height);
#endif