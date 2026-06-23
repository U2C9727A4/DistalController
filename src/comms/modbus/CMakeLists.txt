add_library( modbus
    modbus.c
)

FetchContent_Declare(
        nanomodbus
        GIT_REPOSITORY https://github.com/debevv/nanoMODBUS
        GIT_TAG v1.23.0 # or the version you want
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(nanomodbus)

target_link_libraries(modbus PRIVATE
    FreeRTOS-Kernel-Heap4
    pico_sdk 
    nanomodbus
)

target_include_directories(modbus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})