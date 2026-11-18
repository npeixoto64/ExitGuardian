CMake Configure: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake
Compile: cmake --build build

cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build

cmake -S . -B build-debug -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug


Clean: rm -rf build
Clean: rm -rf build-debug

Flash: stm8flash -c stlinkv2 -p STM8L051F3 -w build/fw.ihx

RESET: stm8flash -c stlinkv2 -p STM8L051F3 -R

Dump from MCU: stm8flash -c stlinkv2 -p STM8L051F3 -r flash.bin -s 0x8000

TL;DR:
    CLEAN: rm -rf build
    COMFIGURE: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-stm8-sdcc.cmake
    BUILD: cmake --build build
    FLASH: stm8flash -c stlinkv2 -p STM8L051F3 -w build/fw.ihx
    RESET: stm8flash -c stlinkv2 -p STM8L051F3 -R

Gateway <-> programmer:
1. VDD -> violet
2. DATA -> grey
3. GND -> white
4. RESET -> black

Gateway <-> PC:
1. RXD (PC - green)
2. TXD (PC - blue) - not needed
3. GND (black)

ToDos:
 - A code refactoring in the product is needed to reduce the main size.
 - Consider design the architecture.
 - Measure the timmings because of windowed watchdog timings. This code is just PoC.
 - Consider 32 KB of flash instead of 16 KB. It will depend on MCU price.
 - Consider UFQFPN instead of TSSOP20 package.
 - Measure the battery current consumption to make sure the MCU and CC1101 are in sleep mode.
 - Decide about the antenna, on board or out of the board, or ceramic.
 - Reduce the CC1101 power.

Unused pins:
PB2 - GND
PB3 - GND
PC0 - GND
PC1 - GND
PC4 - GND

Used pins:
SPI:
PB4 -> CS
PB5 -> SCK
PB6 -> MOSI
PB7 -> MISO

Inputs:
PA2 -> reed
PA3 -> btn

Radio IRQ:
PB1 -> GDO0

LED:
PB0 -> RED LED

Serial:
PC5 -> TX
PC6 -> RX
