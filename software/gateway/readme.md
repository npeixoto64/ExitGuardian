CMake Configure: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake
Compile: cmake --build build

Clean: rm -rf build
