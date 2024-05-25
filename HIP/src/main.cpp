#include <hip/hip_runtime.h>

#include <signal.h>
#include <unistd.h>
#include <time.h> 

#include <GL/freeglut.h>

#include "constants.h"
#include "kernels.h"

// HIP Error checking
#define HIP_ASSERT(x) (assert((x)==hipSuccess))

//Interrupt handler
bool interrupted = false;

void intHandler(int dummy) {
    interrupted = true;
}

void intHandlerVoid(void) {
    interrupted = true;
}

// CPU Timers for Debugging
#define DEBUG_PRINT false

#if DEBUG_PRINT
timespec start_CPU = {};
void start_CPU_timer(){
    start_CPU = std::chrono::high_resolution_clock::now();
}

long stop_CPU_timer(const char* info){
    auto elapsed = std::chrono::high_resolution_clock::now() - start_CPU;
    long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    printf("%ld microseconds\t\t%s\n", microseconds, info);
    return microseconds;
}
#else
void start_CPU_timer(){}
long stop_CPU_timer(const char* info){return 0;}
#endif

// ============================================================================
// ============================== OpenGL ======================================
// ============================================================================

// GLUT Setup
void setUpOpenGL(int argc, char** argv) {
    const int width = argc > 1 ? atoi(argv[1]) : 1920;
    const int height = argc > 2 ? atoi(argv[2]) : 1080;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutCreateWindow("Game of Life (HIP)");
    atexit(intHandlerVoid);
}

void renderOpenGL(DATATYPE* frameBuf, int width, int height) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(width, height, GL_LUMINANCE, PIXEL_SIZE_GL, frameBuf);
    glutSwapBuffers();
}

void cleanUpOpenGL() {
    glutDestroyWindow(glutGetWindow());
}

int main(int argc, char** argv) {
    //SDL Setup
    printf("Game of Life (HIP)\n");
    setUpOpenGL(0, NULL);
    // Get width and height from arguments
    const int width = argc > 1 ? atoi(argv[1]) : 1920;
    const int height = argc > 2 ? atoi(argv[2]) : 1080;
    // Initialize random seed
    srand(time(NULL));
    srand(0);

    // Host and device memory
    DATATYPE *h_frameBuf1;
    DATATYPE *d_frameBuf1, *d_frameBuf2;

    // Allocate host memory using malloc
    h_frameBuf1 = (DATATYPE*)malloc(DATA_BYTES);

    // Initialize frameBuf
    const u64 max_value = (1ULL << (sizeof(DATATYPE) * 8));
    for (int i = 0; i < PIXELS; i++) {
        h_frameBuf1[i] = rand() % max_value;
    }

    // Allocate device memory
    HIP_ASSERT(hipMalloc(&d_frameBuf1, DATA_BYTES));
    HIP_ASSERT(hipMalloc(&d_frameBuf2, DATA_BYTES));

    // Copy data to device
    HIP_ASSERT(hipMemcpy(d_frameBuf1, h_frameBuf1, DATA_BYTES, hipMemcpyHostToDevice));

    //Add interupt handler
    signal(SIGINT, intHandler);

    //Timing
    timespec start_time, end_time;
    double frame_time;
    int counter = 0;

    // Kernel launch configuration
    const int threadsPerBlock = 32;
    const int blocksPerGrid = (PIXELS + threadsPerBlock - 1) / threadsPerBlock;

    // Main loop
    while (!interrupted) {
        #if DEBUG_PRINT
            std::cout << "Frame " << counter << std::endl;
        #endif

        //Start FPS timer
        clock_gettime(CLOCK_ID, &start_time);
        start_CPU_timer();
        // Launch kernel
        start_CPU_timer();
        hipLaunchKernelGGL(kernelOfLife, dim3(blocksPerGrid), dim3(threadsPerBlock), 0, 0, d_frameBuf2, d_frameBuf1, PIXELS, width, height);
        //kernelOfLife<<<dim3(blocksPerGrid), dim3(threadsPerBlock), 0, 0>>>(d_frameBuf2, d_frameBuf1, PIXELS, width, height);
        stop_CPU_timer("Kernel launch");

        // Synchronize threads
        HIP_ASSERT(hipDeviceSynchronize());
        stop_CPU_timer("Kernel Done");

        //Copy data to host
        HIP_ASSERT(hipMemcpy(h_frameBuf1, d_frameBuf1, DATA_BYTES, hipMemcpyDeviceToHost));
        
        // Render frame
        start_CPU_timer();
        renderOpenGL(h_frameBuf1, width, height);
        stop_CPU_timer("Render frame");

        //Calulate frame time
        clock_gettime(CLOCK_ID, &end_time);
        frame_time += (end_time.tv_sec - start_time.tv_sec) * 1e9 +
                      (end_time.tv_nsec - start_time.tv_nsec);
        if (++counter == 60)
        {   
            printf("Frame time: %fns, FPS: %f\n", frame_time/counter, (1e9*counter)/frame_time);
            //std::cout << "Frame time: " << frame_time/counter << "ns, FPS: " << (1e9*counter)/frame_time << std::endl;
            counter = 0;
            frame_time = 0;
        }
        
        // Swap buffers
        start_CPU_timer();
        DATATYPE* temp = d_frameBuf1;
        d_frameBuf1 = d_frameBuf2;
        d_frameBuf2 = temp;
        stop_CPU_timer("Swap buffers");

        // Handle events
        start_CPU_timer();
        glutMainLoopEvent();
        stop_CPU_timer("Main loop event");

        #if DEBUG_PRINT
        printf("\n");
        //Sleep for 1 second
        usleep(1000000);
        #endif
    }
    printf("Exiting...\n");
    
    // Free device memory
    HIP_ASSERT(hipFree(d_frameBuf1));
    HIP_ASSERT(hipFree(d_frameBuf2));

    // Free host memory
    free(h_frameBuf1);

    // Clean up SDL
    cleanUpOpenGL();

    return 0;
}
