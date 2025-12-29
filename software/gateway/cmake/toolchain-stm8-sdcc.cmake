set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR stm8)

# We won't use CMake's built-in compile/link rules for SDCC STM8,
# but set these for discovery / IDE integration.
find_program(SDCC sdcc REQUIRED)
find_program(SDASSTM8 sdasstm8 REQUIRED)

set(CMAKE_C_COMPILER ${SDCC})
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

