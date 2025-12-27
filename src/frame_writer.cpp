#include "frame_writer.h"
#include <fstream>

void FrameWriter::writeBMP(const char* filename, FrameBuffer<GrayPixel>* buffer) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Output file creation failed." << std::endl;
            return;
        }

        int width = buffer->getWidth();
        int height = buffer->getHeight();
        int paddingSize = (4 - (width * 3) % 4) % 4;
        int fileSize = 54 + (width * 3 + paddingSize) * height;

        // --- WRITE HEADER ---
        unsigned char header[54] = {0};
        header[0] = 'B'; header[1] = 'M';
        *(int*)&header[2] = fileSize;
        *(int*)&header[10] = 54;
        *(int*)&header[14] = 40;
        *(int*)&header[18] = width;
        *(int*)&header[22] = height;
        *(short*)&header[26] = 1;
        *(short*)&header[28] = 24; // We write 24-bit for compatibility

        file.write((char*)header, 54);

        // --- WRITE PIXEL DATA ---
        unsigned char pad[3] = {0, 0, 0};

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint8_t gray = buffer->getPixel(x, y);
                // Write 3 times (B, G, R) to simulate grayscale in 24-bit format
                file.write((char*)&gray, 1);
                file.write((char*)&gray, 1);
                file.write((char*)&gray, 1);
            }
            file.write((char*)pad, paddingSize);
        }

        file.close();
        #ifdef DEBUG
        std::cout << "Output Writer: Saved " << filename << std::endl;
        #endif
};