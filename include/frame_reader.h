#ifndef FRAME_READER_H
#define FRAME_READER_H

// Hardware Constraints
#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080

#include <iostream>
#include <fstream>
#include "image_types.h"
#include "buffer.h"

class FrameReader {
public:
    // Reads a BMP file and returns a pointer to a new FrameBuffer
    FrameBuffer<Pixel>* readBMP(const char* filename);
};

#endif