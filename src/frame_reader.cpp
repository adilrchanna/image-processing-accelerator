#include "frame_reader.h"
#include <iostream>
#include <fstream>

FrameBuffer<Pixel>* FrameReader::readBMP(const char* filename) {
        std::ifstream file(filename, std::ios::binary);

        if (!file) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return nullptr;
        }

        // 1. READ HEADER (54 Bytes)
        unsigned char header[54];
        file.read((char*)header, 54);

        if (header[0] != 'B' || header[1] != 'M') {
            std::cerr << "Error: Not a valid BMP file." << std::endl;
            return nullptr;
        }

        // Extract Width (at byte 18) and Height (at byte 22)
        int width  = *(int*)&header[18];
        int height = *(int*)&header[22];
        
        // --- NEW HARDWARE CONSTRAINTS ---
        
        // Constraint 1: BRAM Limit
        if (width > MAX_WIDTH || height > MAX_HEIGHT) {
            std::cerr << "Hardware Error: Input resolution (" << width << "x" << height 
                      << ") exceeds FPGA BRAM limits (" << MAX_WIDTH << "x" << MAX_HEIGHT << ")!" << std::endl;
            return nullptr;
        }

        // Constraint 2: Bus Alignment
        // In hardware, reading unaligned rows causes massive bus stalls.
        if (width % 4 != 0) {
            std::cerr << "Hardware Error: Image width (" << width 
                      << ") must be aligned to 4 bytes (AXI Bus constraint)." << std::endl;
            return nullptr;
        }
        // -------------------------------

        // Check bit depth (at byte 28) - must be 24-bit
        short bitDepth = *(short*)&header[28];
        if (bitDepth != 24) {
            std::cerr << "Error: Only 24-bit BMPs are supported." << std::endl;
            return nullptr;
        }

        #ifdef DEBUG
        std::cout << "Input Detected: " << width << "x" << height << " (24-bit)" << std::endl;
        #endif

        // 2. ALLOCATE MEMORY
        FrameBuffer<Pixel>* buffer = new FrameBuffer<Pixel>(width, height);
        Pixel* rawData = buffer->getRawData();

        // 3. CALCULATE PADDING (BMP rows must be multiples of 4 bytes)
        int padding = (4 - (width * 3) % 4) % 4;

        // 4. READ PIXEL DATA
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned char color[3];
                file.read((char*)color, 3);

                // BMP stores as BGR, we swap to RGB for our system
                int index = y * width + x;
                rawData[index].b = color[0];
                rawData[index].g = color[1];
                rawData[index].r = color[2];
            }
            // Skip padding bytes
            file.ignore(padding);
        }

        file.close();
        #ifdef DEBUG
        std::cout << "File loaded successfully into RAM." << std::endl;
        #endif
        return buffer;

};
