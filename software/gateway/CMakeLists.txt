cmake_minimum_required(VERSION 3.20)
project(stm8_fw C)

find_program(SDCC sdcc REQUIRED)

# Default build type (useful for single-config generators like Makefiles)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(OUT fw)

# ---- Common flags
set(CFLAGS_COMMON
  -mstm8
  --std-c11
)

set(LDFLAGS_COMMON
  -mstm8
  --code-loc 0x8000
  --data-loc 0x0000
  --out-fmt-ihx
)

# ---- Per-config flags
set(CFLAGS_DEBUG
  --debug
  --fverbose-asm
  --noinvariant
)

set(LDFLAGS_DEBUG
  # Generate a map file from the linker (aslink)
  -Wl-m
)

set(CFLAGS_RELEASE
  --opt-code-size
)

set(LDFLAGS_RELEASE
  # nothing extra for now
)

# ---- Select flags based on build type
string(TOLOWER "${CMAKE_BUILD_TYPE}" _bt)
if(_bt STREQUAL "debug")
  set(CFLAGS  ${CFLAGS_COMMON}  ${CFLAGS_DEBUG})
  set(LDFLAGS ${LDFLAGS_COMMON} ${LDFLAGS_DEBUG})
elseif(_bt STREQUAL "release")
  set(CFLAGS  ${CFLAGS_COMMON}  ${CFLAGS_RELEASE})
  set(LDFLAGS ${LDFLAGS_COMMON} ${LDFLAGS_RELEASE})
else()
  message(WARNING "Unknown CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}'. Using common flags only.")
  set(CFLAGS  ${CFLAGS_COMMON})
  set(LDFLAGS ${LDFLAGS_COMMON})
endif()

# ---- Sources (C only, SDCC provides startup/vectors)
set(SRC_C
  ${CMAKE_SOURCE_DIR}/src/main.c
)

set(IHX ${CMAKE_BINARY_DIR}/${OUT}.ihx)

# Link directly from sources (most reliable with SDCC)
add_custom_command(
  OUTPUT ${IHX}
  COMMAND ${SDCC} ${CFLAGS} ${LDFLAGS} ${SRC_C} -o ${IHX}
  DEPENDS ${SRC_C}
  VERBATIM
)

add_custom_target(${OUT} ALL DEPENDS ${IHX})

# Convenience
add_custom_target(size
  COMMAND ${SDCC} --version
  COMMAND ${CMAKE_COMMAND} -E echo "Built: ${IHX}"
  DEPENDS ${IHX}
)
