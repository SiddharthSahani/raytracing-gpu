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
#if !defined(PIXEL_FORMAT__R8G8B8A8) && !defined(PIXEL_FORMAT__R32G32B32A32)
#   define PIXEL_FORMAT__R32G32B32A32
#endif
#ifdef PIXEL_FORMAT__R8G8B8A8
#   define PIXEL_BUFFER__TYPE uchar4
#endif
#ifdef PIXEL_FORMAT__R32G32B32A32
#   define PIXEL_BUFFER__TYPE float4
#endif


#endif
