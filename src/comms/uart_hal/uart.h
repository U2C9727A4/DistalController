#ifndef UARTHAL_H
#define UARTHAL_H
#include <FreeRTOS.h>
#include <task.h>
#include "queue.h"
#include "stream_buffer.h"
#include <hardware/uart.h>
#include <stdint.h>
#include "comms/comms.h"

struct uart_opts_t {
    uint Rxpin;
    uint Txpin;
    uint baud;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity_bits;
};

// Initilize UART
// Assumes the streams on the pipe are initialized.
// on failure, returns true.
// on success, the driver owns the producing end of rx, and consuming end of tx.
// The pipe must have the same lifetime of uart. (Ie deinit() or pipe's close function should be called before pipe goes out of scope)
extern bool uart_drv_init(struct uPipe_t* pipe, struct uart_opts_t opts, uart_inst_t* uart_inst);

// Deinitialize UART0.
extern void uart_drv_deinit(struct uPipe_t* consumed);

#endif