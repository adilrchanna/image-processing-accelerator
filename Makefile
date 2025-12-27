# Makefile for Hardware Accelerator Simulator

# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude

# Target Executable Name
TARGET = ha

# Source Files - Includes all .cpp files
SRCS = src/main.cpp src/frame_reader.cpp src/frame_writer.cpp src/color_converter.cpp src/convolution.cpp

# Build Rules
all: $(TARGET)

$(TARGET): $(SRCS)
	@echo "Building Hardware Simulator..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
	@echo "Build Complete. Run ./$(TARGET)"

# Debug Build Rule
debug: CXXFLAGS += -DDEBUG
debug: clean $(TARGET)

# Fixed-Point Build Rule
fixed: CXXFLAGS += -DUSE_FIXED_POINT
fixed: clean $(TARGET)

# Floating-Point Build Rule
float: CXXFLAGS += -DUSE_FLOAT
float: clean $(TARGET)

# Clean
clean:
	rm -f $(TARGET) *.o output_*.bmp input_*.bmp
