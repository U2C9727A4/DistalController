add_library(uart_hal
    uart.c
)

target_link_libraries(uart_hal PRIVATE
    FreeRTOS-Kernel-Heap4
    pico_sdk 
)

target_include_directories(uart_hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})