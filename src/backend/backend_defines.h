
#ifdef RT_BACKEND_RAYLIB
#define RT_BACKEND_PRESENT
#endif


// if using raylib, then no need to define it
#ifndef RT_BACKEND_RAYLIB
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
