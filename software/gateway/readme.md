CMake Configure: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake
Compile: cmake --build build

Clean: rm -rf build

Flash: stm8flash -c stlinkv2 -p stm8l151k4 -w build/fw.ihx

Dump from MCU: stm8flash -c stlinkv2 -p stm8l151k4 -r flash.bin -s 0x8000