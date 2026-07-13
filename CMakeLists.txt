# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.20)
set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Board type")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG 2.3.0)
include(${CMAKE_CURRENT_SOURCE_DIR}/pico_sdk_import.cmake)
include(FetchContent)
set(PICO_PLATFORM "rp2040")

pico_sdk_init()

project(DistalController)



set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_CONFIG_DIR ${CMAKE_CURRENT_LIST_DIR}/src/)


add_library(freertos_config INTERFACE)
set(FREERTOS_PORT "GCC_RP2040" CACHE STRING "")

target_include_directories(freertos_config SYSTEM INTERFACE
    ${FREERTOS_CONFIG_DIR}
)

target_compile_definitions(freertos_config
  INTERFACE
  projCOVERAGE_TEST=0
)
FetchContent_Declare( freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        V11.3.0
)

FetchContent_MakeAvailable(freertos_kernel)

set(FREERTOS_KERNEL_PATH ${freertos_kernel_SOURCE_DIR})


#include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/lispbm_import.cmake)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)