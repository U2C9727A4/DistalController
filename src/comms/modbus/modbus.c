// stub
#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "stream_buffer.h"
#include "nanomodbus.h"
#include <stdint.h>


static StreamBufferHandle_t rx = NULL;
static StreamBufferHandle_t tx = NULL;

int32_t nmbs_read(uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {

    TickType_t timeout = 0;
    if (byte_timeout_ms >= 0) timeout = pdMS_TO_TICKS(byte_timeout_ms);
    if (byte_timeout_ms < 0) timeout = portMAX_DELAY;

    return xStreamBufferReceive(rx, buf, count, timeout);
}

int32_t nmbs_write(const uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {
    TickType_t timeout = 0;
    if (byte_timeout_ms >= 0) timeout = pdMS_TO_TICKS(byte_timeout_ms);
    if (byte_timeout_ms < 0) timeout = portMAX_DELAY;

    return xStreamBufferSend(tx, buf, count, timeout);
}