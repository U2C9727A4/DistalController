add_library(common
    common.c
)

target_link_libraries(common PRIVATE
    FreeRTOS-Kernel-Heap4
    pico_stdlib
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})