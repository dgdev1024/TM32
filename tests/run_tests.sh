#!/bin/bash
# TM32 Test Script

echo "TM32 Virtual CPU Emulator Test Suite"
echo "===================================="

# Build the project
echo "Building TM32..."
make clean && make

if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "✅ Build successful"

# Run built-in test
echo ""
echo "Running built-in test..."
./tm32 test

if [ $? -ne 0 ]; then
    echo "❌ Built-in test failed"
    exit 1
fi

echo "✅ Built-in test passed"

# Test assembler with example files
echo ""
echo "Testing assembler with example files..."
./tm32 asm examples/simple.s simple.o

if [ $? -ne 0 ]; then
    echo "❌ Assembly failed"
    exit 1
fi

echo "✅ Assembly successful"

# Test linker
echo ""
echo "Testing linker..."
./tm32 link simple.o simple.exe

if [ $? -ne 0 ]; then
    echo "❌ Linking failed"
    exit 1
fi

echo "✅ Linking successful"

# Test emulator
echo ""
echo "Testing emulator..."
./tm32 run simple.exe

if [ $? -ne 0 ]; then
    echo "❌ Emulation failed"
    exit 1
fi

echo "✅ Emulation successful"

# Cleanup
rm -f simple.o simple.exe

echo ""
echo "🎉 All tests passed!"
echo "TM32 Virtual CPU Emulator is working correctly."