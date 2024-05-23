//Header guards
#ifndef CONSTANTS_H
#define CONSTANTS_H

//Better types
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long

#define true 1
#define false 0

#define CLOCK_ID CLOCK_BOOTTIME

//CONSTANTS
#define BYTES_PER_PIXEL 1

#if BYTES_PER_PIXEL == 1
    #define DATATYPE u8
    #define PIXEL_SIZE_GL GL_UNSIGNED_BYTE
#elif BYTES_PER_PIXEL == 2
    #define DATATYPE u16
    #define PIXEL_SIZE_GL GL_UNSIGNED_SHORT
#elif BYTES_PER_PIXEL == 4
    #define DATATYPE u32
    #define PIXEL_SIZE_GL GL_UNSIGNED_INT
#elif BYTES_PER_PIXEL == 8
    #define DATATYPE u64
    //TODO: Find correct GL type for 64 bit
    #define PIXEL_SIZE_GL GL_UNSIGNED_INT
#endif

#define BLACK (DATATYPE)0
#define WHITE (DATATYPE)-1

#define PIXELS (width * height)
#define DATA_BITS (PIXELS * sizeof(DATATYPE) * 8)
#define DATA_BYTES (PIXELS * sizeof(DATATYPE))
#endif