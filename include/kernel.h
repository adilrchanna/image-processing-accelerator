#ifndef KERNEL_H
#define KERNEL_H

#include <cstdint>

// Default to Fixed Point if nothing is defined
#if !defined(USE_FIXED_POINT) && !defined(USE_FLOAT)
#define USE_FIXED_POINT
#endif

#ifdef USE_FIXED_POINT
    // --- FIXED POINT MODE (Hardware) ---
    // A 3x3 Matrix Structure for Convolution (Integer)
    struct Kernel {
        int16_t weights[3][3];
        uint8_t shift; // Bit shift for division
        int16_t bias;  // Brightness offset
    };

    // 1. Box Blur (Average 3x3)
    // Weights 58 -> Sum 522. Shift 9 (512). Gain ~1.02.
    // Note: User requested 58s.
    const Kernel k_blur = {
        {
            {58, 58, 58},
            {58, 58, 58},
            {58, 58, 58}
        },
        9, 
        0
    };

    // 2. Sharpen (Enhance Edges)
    // Scaled by 512 (Shift 9).
    const Kernel k_sharpen = {
        {
            {   0, -512,    0},
            {-512, 2560, -512},
            {   0, -512,    0}
        },
        9, 0
    };

    // 3. Gaussian Blur (The "Pro" Smoother)
    // Scaled by 32 to reach sum 512 (Shift 9).
    const Kernel k_gaussian = {
        {
            {32,  64, 32},
            {64, 128, 64},
            {32,  64, 32}
        },
        9, 0
    };

    // 4. Sobel X (Detects Vertical Edges)
    const Kernel k_sobel_x = {
        {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}},
        0, 0
    };

    // 5. Sobel Y (Detects Horizontal Edges)
    const Kernel k_sobel_y = {
        {{-1, -2, -1}, { 0,  0,  0}, { 1,  2,  1}},
        0, 0
    };

#else
    // --- FLOAT MODE (Software Verification) ---
    // A 3x3 Matrix Structure for Convolution (Float)
    struct Kernel {
        float weights[3][3];
        float scale; // Division factor
        float bias;  // Brightness offset
    };

    // 1. Box Blur
    const Kernel k_blur = {
        {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
        1.0f / 9.0f, 0.0f
    };

    // 2. Sharpen
    const Kernel k_sharpen = {
        {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}},
        1.0f, 0.0f
    };

    // 3. Gaussian Blur
    const Kernel k_gaussian = {
        {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}},
        1.0f / 16.0f, 0.0f
    };

    // 4. Sobel X
    const Kernel k_sobel_x = {
        {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}},
        1.0f, 0.0f
    };

    // 5. Sobel Y
    const Kernel k_sobel_y = {
        {{-1, -2, -1}, { 0,  0,  0}, { 1,  2,  1}},
        1.0f, 0.0f
    };

#endif

#endif