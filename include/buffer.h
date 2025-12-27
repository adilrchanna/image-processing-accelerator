#ifndef BUFFER_H
#define BUFFER_H

#include "image_types.h"
#include <iostream>
#include <cstdlib> // For malloc and free
#include <cstring> // For memset

template <typename T>
class FrameBuffer {
private:
    int width;
    int height;
    T* data; // The pointer to our physical memory block

public:
    // CONSTRUCTOR (Simulates Memory Allocation / Power On)
    FrameBuffer(int w, int h) : width(w), height(h) {
        size_t totalPixels = width * height;
        size_t totalBytes = totalPixels * sizeof(T);

        // Malloc reserves the physical RAM addresses
        data = (T*)malloc(totalBytes);

        if (!data) {
            std::cerr << "CRITICAL ERROR: Hardware Memory Allocation Failed!" << std::endl;
            exit(1);
        }

        // Initialize memory to 0 (Black) to avoid garbage data
        std::memset(data, 0, totalBytes);
    }

    // DESTRUCTOR (Simulates Memory Freeing / Power Off)
    ~FrameBuffer() {
        if (data) {
            free(data);
            data = nullptr;
        }
    }

    // GETTERS
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // RAW ACCESS (Direct Memory Access)
    T* getRawData() { return data; }

    // HELPER: Set a pixel
    void setPixel(int x, int y, T value) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data[y * width + x] = value;
        }
    }
    
    // HELPER: Get a pixel
    T getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return data[y * width + x];
        }
        return T(); // Return empty if out of bounds
    }
};

#endif // BUFFER_H