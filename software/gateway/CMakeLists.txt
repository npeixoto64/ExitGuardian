cmake_minimum_required(VERSION 3.20)
project(stm8_fw C)

find_program(SDCC sdcc REQUIRED)
find_program(SDASSTM8 sdasstm8 REQUIRED)

set(OUT fw)

set(CFLAGS
  -mstm8
  --std-c11
  --opt-code-size
)

set(LDFLAGS
  -mstm8
  --nostdlib
  --code-loc 0x8000
  --data-loc 0x0000
  -Wl-bCODE=0x8080
  -L/usr/share/sdcc/lib/stm8
  -lstm8
)

set(REL_DIR ${CMAKE_BINARY_DIR}/rel)
file(MAKE_DIRECTORY ${REL_DIR})

# Build in link order: vectors -> startup -> main
#set(SRC_S
#  ${CMAKE_SOURCE_DIR}/src/vectors.s
#  ${CMAKE_SOURCE_DIR}/src/startup.s
#)
set(SRC_C
  ${CMAKE_SOURCE_DIR}/src/main.c
)

set(REL_FILES "")

foreach(SRC ${SRC_S})
  get_filename_component(BASENAME ${SRC} NAME_WE)
  set(OUTREL ${REL_DIR}/${BASENAME}.rel)
  add_custom_command(
    OUTPUT ${OUTREL}
    COMMAND ${SDASSTM8} -l -o ${OUTREL} ${SRC}
    DEPENDS ${SRC}
    VERBATIM
  )
  list(APPEND REL_FILES ${OUTREL})
endforeach()

foreach(SRC ${SRC_C})
  get_filename_component(BASENAME ${SRC} NAME_WE)
  set(OUTREL ${REL_DIR}/${BASENAME}.rel)
  add_custom_command(
    OUTPUT ${OUTREL}
    COMMAND ${SDCC} -c ${CFLAGS} ${SRC} -o ${OUTREL}
    DEPENDS ${SRC}
    VERBATIM
  )
  list(APPEND REL_FILES ${OUTREL})
endforeach()

set(IHX ${CMAKE_BINARY_DIR}/${OUT}.ihx)

add_custom_command(
  OUTPUT ${IHX}
  COMMAND ${SDCC} ${CFLAGS} ${LDFLAGS} ${REL_FILES} -o ${IHX}
  DEPENDS ${REL_FILES}
  VERBATIM
)

add_custom_target(${OUT} ALL DEPENDS ${IHX})
