#ifndef COLOR_CONVERTER_H
#define COLOR_CONVERTER_H

#include "image_types.h"
#include "buffer.h"
#include <iostream>

class ColorConverter {
public:
    void process(FrameBuffer<Pixel>* input, FrameBuffer<GrayPixel>* output);
};

#endif