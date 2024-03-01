
kernel void accumulateFrameData(
    read_only image2d_t frameImage,
    read_only image2d_t accumImage_r,
    write_only image2d_t accumImage_w,
    uint numFrames, uint imgWidth
) {
    size_t pixelIdx = get_global_id(0);
    int2 imgCoords = {pixelIdx % imgWidth, pixelIdx / imgWidth};

    float4 frameColor = read_imagef(frameImage, imgCoords);
    float4 accumColor = read_imagef(accumImage_r, imgCoords);
    float4 avgColor = (accumColor * (numFrames - 1) + frameColor) / numFrames;

    write_imagef(accumImage_w, imgCoords, avgColor);
}
