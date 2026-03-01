CMake Configure: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake
Compile: cmake --build build

cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build

cmake -S . -B build-debug -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug


Clean: rm -rf build
Clean: rm -rf build-debug

Flash: stm8flash -c stlinkv2 -p stm8l151k4 -w build/fw.ihx

RESET: stm8flash -c stlinkv2 -p stm8l151k4 -R

Dump from MCU: stm8flash -c stlinkv2 -p stm8l151k4 -r flash.bin -s 0x8000

TL;DR:
    CLEAN: rm -rf build
    COMFIGURE: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake
    BUILD: cmake --build build
    FLASH: stm8flash -c stlinkv2 -p stm8l151k4 -w build/fw.ihx
    RESET: stm8flash -c stlinkv2 -p stm8l151k4 -R
