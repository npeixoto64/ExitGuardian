cmake_minimum_required(VERSION 3.20)
project(stm8_fw C)

add_subdirectory(hal)

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
  -DSTM8L15X_MD
  -D__SDCC__
  -DUSE_FULL_ASSERT=0
  -I${CMAKE_SOURCE_DIR}/hal/inc
  -I${CMAKE_SOURCE_DIR}/hal/cfg
  -I${CMAKE_SOURCE_DIR}/inc
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

# ---- Sources
set(SRC_C
  ${CMAKE_SOURCE_DIR}/src/main.c
  ${CMAKE_SOURCE_DIR}/src/cc1101.c
  ${CMAKE_SOURCE_DIR}/src/feram.c
  ${CMAKE_SOURCE_DIR}/src/sensor_manager.c
  ${CMAKE_SOURCE_DIR}/src/log.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_clk.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_gpio.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_adc.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_i2c.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_spi.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_usart.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_tim1.c
  ${CMAKE_SOURCE_DIR}/hal/src/stm8l15x_exti.c
)

set(SRC_ALL ${SRC_C} ${SRC_S})

# Compile each source to .rel
set(REL_FILES)
foreach(src ${SRC_ALL})
  get_filename_component(name ${src} NAME_WE)
  get_filename_component(ext ${src} EXT)
  set(rel ${CMAKE_BINARY_DIR}/${name}.rel)
  list(APPEND REL_FILES ${rel})
  if(ext STREQUAL ".s" OR ext STREQUAL ".asm")
    add_custom_command(
      OUTPUT ${rel}
      COMMAND ${SDASSTM8} -plosgff -o ${rel} ${src}
      DEPENDS ${src}
      VERBATIM
    )
  else()
    add_custom_command(
      OUTPUT ${rel}
      COMMAND ${SDCC} -c ${CFLAGS} ${src} -o ${rel}
      DEPENDS ${src}
      VERBATIM
    )
  endif()
endforeach()

set(IHX ${CMAKE_BINARY_DIR}/${OUT}.ihx)

# Link .rel files to .ihx
add_custom_command(
  OUTPUT ${IHX}
  COMMAND ${SDCC} ${LDFLAGS} ${REL_FILES} -o ${IHX}
  DEPENDS ${REL_FILES}
  VERBATIM
)

add_custom_target(${OUT} ALL DEPENDS ${IHX})

# Convenience
add_custom_target(size
  COMMAND ${SDCC} --version
  COMMAND ${CMAKE_COMMAND} -E echo "Built: ${IHX}"
  DEPENDS ${IHX}
)
