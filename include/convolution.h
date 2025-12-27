#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "image_types.h"
#include "buffer.h"
#include "kernel.h"
#include <iostream>
#include <cmath> // abs() works for ints too

class ConvolutionEngine {
public:
    // Standard Filter Process (Fixed Point or Float)
    void process(FrameBuffer<GrayPixel>* input, FrameBuffer<GrayPixel>* output, const Kernel& k);
    void processSobel(FrameBuffer<GrayPixel>* input, FrameBuffer<GrayPixel>* output);
};

#endif