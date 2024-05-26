#ifndef CONSTS_H
#define CONSTS_H

#define STRINGIZE(x) #x
#define STR(x) STRINGIZE(x)

#define WIDTH 1920
#define HEIGHT 1080
#define DIV_FACTOR 32

#define true 1
#define false 0

#define DEBUG true
#define DEVELOPMENT false

//Ensure that the width and height are divisible by the DIV_FACTOR

// THIS IS NOW HANDLED BY THE IF STATEMENT IN THE SHADER
//#include <assert.h>
//static_assert(WIDTH % DIV_FACTOR == 0, "Width must be divisible by Div_factor");
//static_assert(HEIGHT % DIV_FACTOR == 0, "Height must be divisible by Div_factor");

// CPU Timers for Debugging
#define CLOCK_ID CLOCK_MONOTONIC //CLOCK_BOOTTIME


#endif