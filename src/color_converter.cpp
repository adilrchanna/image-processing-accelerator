#include "color_converter.h"
#include <iostream>

// Default to Fixed Point if nothing is defined (Safety)
#if !defined(USE_FIXED_POINT) && !defined(USE_FLOAT)
#define USE_FIXED_POINT
#endif

void ColorConverter::process(FrameBuffer<Pixel>* input, FrameBuffer<GrayPixel>* output) {
    // Input: RGB Buffer (3 bytes)
    // Output: Grayscale Buffer (1 byte)     
    int width = input->getWidth();
    int height = input->getHeight();

    // Hardware Constraint Check
    if (output->getWidth() != width || output->getHeight() != height) {
        std::cerr << "Error: Buffer dimensions mismatch!" << std::endl;
        return;
    }

    #ifdef DEBUG
    #ifdef USE_FIXED_POINT
        std::cout << " [ISP] Fixed-Point Conversion (RGB -> Gray)..." << std::endl;
    #else
        std::cout << " [ISP] Floating-Point Conversion (RGB -> Gray)..." << std::endl;
    #endif
    #endif

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            
            // 1. FETCH (Read from Input Memory)
            Pixel p = input->getPixel(x, y);

            #ifdef USE_FIXED_POINT
                // --- FIXED POINT MODE (Hardware) ---
                // Math: Q8.8 format. 
                // Coefficients scaled by 256. 
                // 0.299 * 256 = 77
                // 0.587 * 256 = 150
                // 0.114 * 256 = 29
                // Sum = 256 (Perfect power of 2)
                
                int32_t gray_accum = (77 * p.r) + (150 * p.g) + (29 * p.b);
                
                // Shift right by 8 (Divide by 256)
                uint8_t gray = (uint8_t)(gray_accum >> 8);
                
                output->setPixel(x, y, gray);

            #else
                // --- FLOATING POINT MODE (Verification) ---
                // Standard Luma Formula
                float gray_f = (0.299f * p.r) + (0.587f * p.g) + (0.114f * p.b);
                
                // Clamp and Cast
                if (gray_f > 255.0f) gray_f = 255.0f;
                if (gray_f < 0.0f)   gray_f = 0.0f;
                
                output->setPixel(x, y, (uint8_t)gray_f);
            #endif
        }
    }
    
    #ifdef DEBUG
    std::cout << " [ISP] Conversion Complete." << std::endl;
    #endif
}