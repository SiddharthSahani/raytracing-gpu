
#include "kernels/defines.h"


kernel void accumulateFrameData(
    global PIXEL_BUFFER__TYPE* accumulatedFrameData,
    const global PIXEL_BUFFER__TYPE* frameData,
    uint numFrames
) {
    size_t i = get_global_id(0);

#ifdef PIXEL_FORMAT__RGBA32FA32
    float4 accumColor = accumulatedFrameData[i];
    float4 unaccumColor = frameData[i];
#endif
#ifdef PIXEL_FORMAT__RGBA8
    float4 accumColor = convert_float4(accumulatedFrameData[i]) / 255.0f;
    float4 unaccumColor = convert_float4(frameData[i]) / 255.0f;
#endif

    float4 averageColor = (accumColor * (numFrames - 1) + unaccumColor) / numFrames;

#ifdef PIXEL_FORMAT__RGBA32FA32
    accumulatedFrameData[i] = averageColor;
    accumulatedFrameData[i].w = 1.0f;
#endif
#ifdef PIXEL_FORMAT__RGBA8
    averageColor = clamp(averageColor, 0.0f, 1.0f);
    accumulatedFrameData[i] = convert_uchar4(averageColor * 255.0f);
#endif

}
