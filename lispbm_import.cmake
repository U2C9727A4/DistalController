set( LISPBM_DIR
    "${CMAKE_CURRENT_LIST_DIR}/LispBM"
)

add_library(LispBM
    "${LISPBM_DIR}/src/env.c"
    "${LISPBM_DIR}/src/eval_cps.c"
    "${LISPBM_DIR}/src/extensions.c"
    "${LISPBM_DIR}/src/fundamental.c"
    "${LISPBM_DIR}/src/heap.c"
    "${LISPBM_DIR}/src/lbm_memory.c"
    "${LISPBM_DIR}/src/lbm_channel.c"
    "${LISPBM_DIR}/src/lbm_c_interop.c"
    "${LISPBM_DIR}/src/lbm_custom_type.c"
    "${LISPBM_DIR}/src/lbm_defrag_mem.c"
    "${LISPBM_DIR}/src/lbm_flat_value.c"
    "${LISPBM_DIR}/src/lbm_image.c"
    "${LISPBM_DIR}/src/print.c"
    "${LISPBM_DIR}/src/stack.c"
    "${LISPBM_DIR}/src/symrepr.c"
    "${LISPBM_DIR}/src/tokpar.c"
    "${LISPBM_DIR}/src/lispbm.c"
    "${LISPBM_DIR}/platform/freertos/src/platform_mutex.c"
    "${LISPBM_DIR}/platform/freertos/src/platform_timestamp.c"
    "${LISPBM_DIR}/platform/freertos/src/platform_thread.c"
)

target_include_directories(LispBM PUBLIC
    "${LISPBM_DIR}/include"
    "${LISPBM_DIR}/platform/freertos/include"
)

target_link_libraries(LispBM PUBLIC
    freertos_kernel
)