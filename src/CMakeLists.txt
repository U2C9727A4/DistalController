add_executable(controller
    boot.c
)

target_include_directories(FreeRTOS-Kernel INTERFACE
    ${CMAKE_CURRENT_LIST_DIR} 
)



target_link_libraries(controller PUBLIC
    pico_stdlib 
    FreeRTOS-Kernel-Heap4 
)



set(subprojects comms sandbox)

foreach(dir IN LISTS subprojects)
    target_include_directories(controller PRIVATE ${dir})
    add_subdirectory(${dir})
endforeach()

target_link_libraries(controller PRIVATE
    comms
)