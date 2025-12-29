cmake_minimum_required(VERSION 3.20)
project(stm8_fw C)

# ---- Tools
find_program(SDCC sdcc REQUIRED)
find_program(SDASSTM8 sdasstm8 REQUIRED)

# ---- Config (adjust if you use bootloader offset / UBC)
set(PROJECT_NAME_OUT fw)

# Vector table at 0x8000 takes 0x80 bytes => code from 0x8080
set(CODE_LOC 0x8080)
set(DATA_LOC 0x0000)

set(CFLAGS
  -mstm8
  --std-c11
  --opt-code-size
)

set(LDFLAGS
  -mstm8
  --code-loc ${CODE_LOC}
  --data-loc ${DATA_LOC}
)

# ---- Sources
set(SRC_C
  ${CMAKE_SOURCE_DIR}/src/main.c
)

set(SRC_S
  ${CMAKE_SOURCE_DIR}/src/startup.s
  ${CMAKE_SOURCE_DIR}/src/vectors.s
)

set(REL_FILES "")

# Output directory for .rel
set(REL_DIR ${CMAKE_BINARY_DIR}/rel)
file(MAKE_DIRECTORY ${REL_DIR})

# Compile C -> .rel
foreach(SRC ${SRC_C})
  get_filename_component(BASENAME ${SRC} NAME_WE)
  set(OUT ${REL_DIR}/${BASENAME}.rel)

  add_custom_command(
    OUTPUT ${OUT}
    COMMAND ${SDCC} -c ${CFLAGS} ${SRC} -o ${OUT}
    DEPENDS ${SRC}
    VERBATIM
  )
  list(APPEND REL_FILES ${OUT})
endforeach()

# Assemble .s -> .rel
foreach(SRC ${SRC_S})
  get_filename_component(BASENAME ${SRC} NAME_WE)
  set(OUT ${REL_DIR}/${BASENAME}.rel)

  add_custom_command(
    OUTPUT ${OUT}
    COMMAND ${SDASSTM8} -l -o ${OUT} ${SRC}
    DEPENDS ${SRC}
    VERBATIM
  )
  list(APPEND REL_FILES ${OUT})
endforeach()

# Link -> .ihx
set(IHX ${CMAKE_BINARY_DIR}/${PROJECT_NAME_OUT}.ihx)

add_custom_command(
  OUTPUT ${IHX}
  COMMAND ${SDCC} ${CFLAGS} ${LDFLAGS} ${REL_FILES} -o ${IHX}
  DEPENDS ${REL_FILES}
  VERBATIM
)

add_custom_target(${PROJECT_NAME_OUT} ALL DEPENDS ${IHX})

# Convenience targets
add_custom_target(size
  COMMAND ${SDCC} --version
  COMMAND ${CMAKE_COMMAND} -E echo "IHX: ${IHX}"
  DEPENDS ${IHX}
)

# Optional flash target (requires tools/flash.sh)
add_custom_target(flash
  COMMAND ${CMAKE_SOURCE_DIR}/tools/flash.sh ${IHX}
  DEPENDS ${IHX}
  USES_TERMINAL
)
