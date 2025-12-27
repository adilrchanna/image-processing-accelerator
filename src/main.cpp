/**
 * @file main.cpp
 * @brief Top-level Synchronous Hardware Pipeline Simulator
 * * This file simulates a 4-stage hardware pipeline:
 * Stage 1: Frame Reader (DMA Input)
 * Stage 2: Color Converter (ISP)
 * Stage 3: Convolution Engine (DSP Accelerator)
 * Stage 4: Frame Writer (DMA Output)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // For std::swap

// Hardware Module Headers
#include "image_types.h"
#include "buffer.h"
#include "frame_reader.h"
#include "frame_writer.h"
#include "color_converter.h"
#include "convolution.h"
#include "kernel.h"

// --- PIPELINE REGISTERS (Inter-Stage Latches) ---
// In hardware, these pointers represent the physical wires/buses 
// connecting the output of one IP block to the input of the next.
FrameBuffer<Pixel>* reg_RawData = nullptr;      
FrameBuffer<GrayPixel>* reg_GrayData = nullptr;     
FrameBuffer<GrayPixel>* reg_ProcessedData = nullptr;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "=== C++ Hardware Accelerator Model ===" << std::endl;
        std::cout << "Usage: ./ha [options] <file1.bmp> <file2.bmp> ..." << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -gaussian    Apply Gaussian Blur" << std::endl;
        std::cout << "  -sharpen     Apply Sharpening" << std::endl;
        std::cout << "  -sobel       Apply Sobel Edge Detection" << std::endl;
        std::cout << "\nNote: Box Blur is always applied as the base filter." << std::endl;
        return 0;
    }

    // 1. Hardware Module Instantiation
    FrameReader reader;
    FrameWriter writer;
    ColorConverter isp;
    ConvolutionEngine dsp;

    // 2. Configuration & State
    bool enable_gaussian = true; 
    bool enable_sharpen  = true; 
    bool enable_sobel    = true; 
    std::vector<std::string> inputFiles;

    // 3. CLI Argument Parsing
    // Determine if the user specified any filters. If so, override defaults.
    bool filter_flag_seen = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-gaussian" || arg == "-sharpen" || arg == "-sobel") {
            filter_flag_seen = true;
            break;
        }
    }

    if (filter_flag_seen) {
        enable_gaussian = enable_sharpen = enable_sobel = false;
    }

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-gaussian") enable_gaussian = true;
        else if (arg == "-sharpen") enable_sharpen = true;
        else if (arg == "-sobel")   enable_sobel = true;
        else if (arg[0] != '-') {
            inputFiles.push_back(arg); 
        }
    }

    int totalFrames = inputFiles.size();
    if (totalFrames == 0) {
        std::cerr << "Error: No valid input .bmp files detected in arguments." << std::endl;
        return 1;
    }

    // Print Architectural Setup
    std::cout << "PGA Pipeline Initialized..." << std::endl;
    std::cout << " [MODE] " << 
    #ifdef USE_FIXED_POINT
        "FIXED-POINT MODE"
    #else
        "FLOATING-POINT MODE"
    #endif
    << std::endl;
    std::cout << " [CONF] Processing " << totalFrames << " frame(s)." << std::endl;
    std::cout << " [CONF] Box Blur: ALWAYS ON" << std::endl;
    std::cout << " [CONF] Gaussian: " << (enable_gaussian ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << " [CONF] Sharpen:  " << (enable_sharpen ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << " [CONF] Sobel:    " << (enable_sobel ? "ENABLED" : "DISABLED") << std::endl;

    int clockCycle = 0;
    int inputIdx = 0;
    int outputIdx = 0;

    // 4. MAIN SYNCHRONOUS CLOCK LOOP
    // We execute stages in REVERSE order (4 -> 3 -> 2 -> 1) to accurately 
    // simulate clock-cycle data latching without race conditions.
    while (outputIdx < totalFrames) {
        
        #ifdef DEBUG
        std::cout << "\n--- CLK Cycle " << clockCycle << " ---" << std::endl;
        #endif

        // --- STAGE 4: OUTPUT (DMA Write-Back) ---
        if (reg_ProcessedData != nullptr) {
            std::string outName = "output_" + std::to_string(outputIdx) + ".bmp";
            #ifdef DEBUG
            std::cout << " [STG 4] Writing " << outName << std::endl;
            #endif
            writer.writeBMP(outName.c_str(), reg_ProcessedData);
            
            // Simulates freeing the hardware buffer after DMA completion
            delete reg_ProcessedData; 
            reg_ProcessedData = nullptr;
            outputIdx++;
        }

        // --- STAGE 3: DSP ACCELERATOR (Convolution) ---
        if (reg_GrayData != nullptr) {
            #ifdef DEBUG
            std::cout << " [STG 3] Running Filter Pipeline" << std::endl;
            #endif
            int w = reg_GrayData->getWidth();
            int h = reg_GrayData->getHeight();
            
            // Ping-pong buffer management within the accelerator
            FrameBuffer<GrayPixel>* src = reg_GrayData; 
            FrameBuffer<GrayPixel>* dst = new FrameBuffer<GrayPixel>(w, h);

            // Base Filtering (Mandatory Box Blur)
            dsp.process(src, dst, k_blur);
            std::swap(src, dst); 

            // Extended Filtering
            if (enable_gaussian) { 
                dsp.process(src, dst, k_gaussian); 
                std::swap(src, dst); 
            }
            if (enable_sharpen) { 
                dsp.process(src, dst, k_sharpen); 
                std::swap(src, dst); 
            }
            if (enable_sobel) { 
                dsp.processSobel(src, dst); 
                std::swap(src, dst); 
            }

            delete dst; // Cleanup the swap buffer
            reg_GrayData = nullptr; 
            reg_ProcessedData = src; // Latch result into the output register
        }

        // --- STAGE 2: ISP (Color Space Conversion) ---
        if (reg_RawData != nullptr) {
            #ifdef DEBUG
            std::cout << " [STG 2] Converting RGB -> Gray" << std::endl;
            #endif
            FrameBuffer<GrayPixel>* grayOut = new FrameBuffer<GrayPixel>(
                reg_RawData->getWidth(), 
                reg_RawData->getHeight()
            );
            
            isp.process(reg_RawData, grayOut);
            
            delete reg_RawData; // Drain the raw input buffer
            reg_RawData = nullptr;
            reg_GrayData = grayOut; // Latch into DSP register
        }

        // --- STAGE 1: INPUT (Frame Reader) ---
        if (inputIdx < totalFrames) {
            #ifdef DEBUG
            std::cout << " [STG 1] Loading " << inputFiles[inputIdx] << std::endl;
            #endif
            FrameBuffer<Pixel>* newFrame = reader.readBMP(inputFiles[inputIdx].c_str());
            
            if (newFrame) {
                reg_RawData = newFrame; // Latch into ISP register
                inputIdx++;
            } else {
                std::cerr << " [STG 1] Fatal: Could not read file. Draining pipeline." << std::endl;
                totalFrames = inputIdx; 
            }
        }
        
        clockCycle++;
    }

    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << " Total Clock Cycles: " << clockCycle << std::endl;
    std::cout << " Frames Processed:   " << outputIdx << std::endl;
    std::cout << " Results saved!" << std::endl;

    return 0;
}
