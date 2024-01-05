// define macros that are going to be used in other files

#ifndef DEFINES_CL_H
#define DEFINES_CL_H


// config
#ifndef CONFIG__SAMPLE_COUNT
#   define CONFIG__SAMPLE_COUNT 1
#endif
#ifndef CONFIG__BOUNCE_LIMIT
#   define CONFIG__BOUNCE_LIMIT 1
#endif

// pixel formats
#if !defined(PIXEL_FORMAT__RGBA8) && !defined(PIXEL_FORMAT__RGBA32FA32)
#   define PIXEL_FORMAT__RGBA32FA32
#endif
#ifdef PIXEL_FORMAT__RGBA8
#   define PIXEL_BUFFER__TYPE uchar4
#endif
#ifdef PIXEL_FORMAT__RGBA32FA32
#   define PIXEL_BUFFER__TYPE float4
#endif


#endif
