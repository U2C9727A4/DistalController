add_executable(controller
    boot.c
)



target_link_libraries(controller PUBLIC
    pico_stdlib
    freertos_kernel
)



set(subprojects comms sandbox common)

foreach(dir IN LISTS subprojects)
    target_include_directories(controller PRIVATE ${dir})
    add_subdirectory(${dir})
endforeach()

target_link_libraries(controller PRIVATE
    comms
    sandbox
    common
)

pico_add_extra_outputs(controller)
