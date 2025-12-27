#include "convolution.h"
#include <iostream>
#include <cmath> // abs() works for ints too

void ConvolutionEngine::process(FrameBuffer<GrayPixel>* input, FrameBuffer<GrayPixel>* output, const Kernel& k) {
        int w = input->getWidth();
        int h = input->getHeight();
        
        #ifdef DEBUG
        #ifdef USE_FIXED_POINT
            std::cout << " [DSP] Fixed-Point Convolution..." << std::endl;
        #else
            std::cout << " [DSP] Floating-Point Convolution..." << std::endl;
        #endif
        #endif

        for (int y = 1; y < h - 1; y++) {
            for (int x = 1; x < w - 1; x++) {
                
                #ifdef USE_FIXED_POINT
                    // --- FIXED POINT MODE ---
                    // Use 32-bit int accumulator to prevent overflow
                    int32_t sum = 0;

                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            uint8_t val = input->getPixel(x + kx, y + ky);
                            sum += (int16_t)val * k.weights[ky + 1][kx + 1];
                        }
                    }

                    // Apply Bit Shift (Hardware Division)
                    if (k.shift > 0) {
                        sum = sum >> k.shift; 
                    }
                    
                    sum += k.bias;

                    // Clamp
                    if (sum < 0) sum = 0;
                    if (sum > 255) sum = 255;

                    output->setPixel(x, y, (uint8_t)sum);

                #else
                    // --- FLOATING POINT MODE ---
                    float sum = 0.0f;

                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            uint8_t val = input->getPixel(x + kx, y + ky);
                            sum += (float)val * k.weights[ky + 1][kx + 1];
                        }
                    }

                    // Apply Scale (Float Multiply)
                    sum = (sum * k.scale) + k.bias;

                    // Clamp
                    if (sum < 0.0f) sum = 0.0f;
                    if (sum > 255.0f) sum = 255.0f;

                    output->setPixel(x, y, (uint8_t)sum);
                #endif
            }
        }
    }

    // Sobel Magnitude (Fixed Point or Float)
    void ConvolutionEngine::processSobel(FrameBuffer<GrayPixel>* input, FrameBuffer<GrayPixel>* output) {
        int w = input->getWidth();
        int h = input->getHeight();
        
        #ifdef DEBUG
        #ifdef USE_FIXED_POINT
            std::cout << " [DSP] Fixed-Point Sobel..." << std::endl;
        #else
            std::cout << " [DSP] Floating-Point Sobel..." << std::endl;
        #endif
        #endif
        
        for (int y = 1; y < h - 1; y++) {
            for (int x = 1; x < w - 1; x++) {
                
                #ifdef USE_FIXED_POINT
                    // --- FIXED POINT MODE ---
                    int32_t sumX = 0;
                    int32_t sumY = 0;

                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            uint8_t val = input->getPixel(x + kx, y + ky);
                            sumX += (int16_t)val * k_sobel_x.weights[ky + 1][kx + 1];
                            sumY += (int16_t)val * k_sobel_y.weights[ky + 1][kx + 1];
                        }
                    }

                    // Hardware Magnitude: |x| + |y|
                    int32_t mag = std::abs(sumX) + std::abs(sumY);

                    if (mag > 255) mag = 255;
                    output->setPixel(x, y, (uint8_t)mag);

                #else
                    // --- FLOATING POINT MODE ---
                    float sumX = 0.0f;
                    float sumY = 0.0f;

                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            uint8_t val = input->getPixel(x + kx, y + ky);
                            sumX += (float)val * k_sobel_x.weights[ky + 1][kx + 1];
                            sumY += (float)val * k_sobel_y.weights[ky + 1][kx + 1];
                        }
                    }

                    // Standard Magnitude: |x| + |y| (Or sqrt(x^2 + y^2))
                    float mag = std::abs(sumX) + std::abs(sumY);

                    if (mag > 255.0f) mag = 255.0f;
                    output->setPixel(x, y, (uint8_t)mag);
                #endif
            }
        }
    };
