# HAL (Hardware Abstraction Layer) for STM8L15x
# Header-only library

add_library(hal INTERFACE)

target_include_directories(hal INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg
)

# Optionally, add headers to the target for IDE support
file(GLOB HAL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg/*.h
)

target_sources(hal INTERFACE ${HAL_HEADERS})
