#ifndef FRAME_WRITER_H
#define FRAME_WRITER_H

#include <fstream>
#include "image_types.h"
#include "buffer.h"

class FrameWriter {
public:
    void writeBMP(const char* filename, FrameBuffer<GrayPixel>* buffer);
};

#endif