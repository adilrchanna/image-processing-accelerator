#ifndef IMAGE_TYPES_H
#define IMAGE_TYPES_H

#include <cstdint> // Needs this for uint8_t

// HARDWARE DEFINITION:
// A struct guarantees these 3 bytes are packed together in memory.
// This mimics a 24-bit pixel bus (R, G, B wires).
struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// A typedef for Grayscale to make our code clearer later.
typedef int32_t FixedPoint; // 32-bit integer for math accumulator
// In hardware, this is an 8-bit bus.
typedef uint8_t GrayPixel;

#endif // IMAGE_TYPES_H