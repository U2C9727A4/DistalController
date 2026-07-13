add_library(comms
    comms.c
)

set(subprojects uart_hal modbus)

foreach(dir IN LISTS subprojects)
    add_subdirectory(${dir})
endforeach()

target_include_directories(comms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(comms PUBLIC
    pico_stdlib
    freertos_kernel 
)