add_library(sandbox
    sandbox.c
)

target_link_libraries(sandbox PRIVATE
    LispBM
    freertos_kernel
    pico_stdlib
)

target_include_directories(sandbox PUBLIC ${CMAKE_CURRENT_LIST_DIR})