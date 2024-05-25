#include <hip/hip_runtime.h>
#include "kernels.h"
#include "constants.h"


// Kernel BYTE Game of Life
__global__ void kernelOfLife(DATATYPE* lastFrame, DATATYPE* nextFrame, const int n, const int width, const int height) {
    const int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        const int x = i % width;
        const int y = i / width;
        int neighbors = 0;

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                const int nx = x + dx;
                const int ny = y + dy;
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                if (lastFrame[ny * width + nx]) neighbors++;
            }
        }

        const int cellAlive = lastFrame[i];
        if (cellAlive) {
            if (neighbors < 2 || neighbors > 3) {
                nextFrame[i] = BLACK; // Cell dies
            } else {
                nextFrame[i] = WHITE; // Cell stays alive
            }
        } else {
            if (neighbors == 3) {
                nextFrame[i] = WHITE; // Cell becomes alive
            } else {
                nextFrame[i] = BLACK; // Cell stays dead
            }
        }
    }
} 

// Kernel BIT Game of Life
__global__ void kernelOfLifeBIT(DATATYPE* lastFrame, DATATYPE* nextFrame, int n, int width, int height) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        int bitsPerInt = sizeof(DATATYPE) * 8; // Number of bits in DATATYPE
        int bitIndex = i * bitsPerInt;

        for (int bit = 0; bit < bitsPerInt && bitIndex + bit < width * height; ++bit) {
            int x = (bitIndex + bit) % width;
            int y = (bitIndex + bit) / width;
            int neighbors = 0;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

                    int neighborIndex = ny * width + nx;
                    int neighborBit = neighborIndex % bitsPerInt;
                    int neighborArrayIndex = neighborIndex / bitsPerInt;
                    if ((lastFrame[neighborArrayIndex] >> neighborBit) & 1) neighbors++;
                }
            }

            const int cellAlive = (lastFrame[i] >> bit) & 1;
            if (cellAlive) {
                if (neighbors < 2 || neighbors > 3) {
                    nextFrame[i] &= ~(1 << bit); // Cell dies
                } else {
                    nextFrame[i] |= (1 << bit); // Cell stays alive
                }
            } else {
                if (neighbors == 3) {
                    nextFrame[i] |= (1 << bit); // Cell becomes alive
                } else {
                    nextFrame[i] &= ~(1 << bit); // Cell stays dead
                }
            }
        }
    }
    //__syncthreads();
}
